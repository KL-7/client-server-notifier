#include <QtNetwork>
#include <QtDebug>

#include "messagereceivingthread.h"


MessageReceivingThread::MessageReceivingThread(int socketDescriptor, quint16 clientId, QObject* parent)
    : QThread(parent), clientId(clientId), socketDescriptor(socketDescriptor), messageSize(0) {
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
}

void MessageReceivingThread::run() {
    qDebug() << "client: new MessageReceivingThread";

    socket = new QSslSocket();
    socket->setPeerVerifyMode(QSslSocket::VerifyNone);

    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(quit()));

//    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
//            this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
//    connect(socket, SIGNAL(modeChanged(QSslSocket::SslMode)),
//            this, SLOT(onModeChanged(QSslSocket::SslMode)));

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        emit error(socket->errorString());
        return;
    }

    socket->startClientEncryption();

    connect(socket, SIGNAL(encrypted()), this, SLOT(readMessage()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(processSocketError(QAbstractSocket::SocketError)));

    exec();

    qDebug() << "client: MessageReceivingThread exit";
}

void MessageReceivingThread::processSocketError(QAbstractSocket::SocketError) {
    if (socket->error() != QAbstractSocket::RemoteHostClosedError) {
        emit error(socket->errorString());
    }
}

void MessageReceivingThread::readMessage() {
    connect(socket, SIGNAL(readyRead()), this, SLOT(readMessage()));

    qDebug() << "client: is " << (socket->isEncrypted() ? "encrypted" : "non-encrypted");

    QDataStream in(socket);

    if (messageSize == 0) {
        if (socket->bytesAvailable() < (int)sizeof(quint16)) {
            return;
        }

        in >> messageSize;
    }

    if (socket->bytesAvailable() < messageSize) {
        return;
    }

    quint16 messageClientId;
    QString message;

    in >> messageClientId;
    in >> message;

    qDebug() << "client: message read";

    QDataStream out(socket);
    if (messageClientId != clientId) {
        out << (quint8)0;
        emit error(tr("Wrong client id %1. Message ignored.").arg(messageClientId));
    } else {
        out << (quint8)1;
        emit messageReceived(message);
    }

//    socket->waitForDisconnected(1000);
    socket->disconnect();
    exit();
}

void MessageReceivingThread::onModeChanged(QSslSocket::SslMode mode) {
    qDebug() << mode;
}

void MessageReceivingThread::onStateChanged(QAbstractSocket::SocketState state) {
    qDebug() << state;
}
