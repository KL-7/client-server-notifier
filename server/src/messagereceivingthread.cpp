#include <QtDebug>

#include "messagereceivingthread.h"


MessageReceivingThread::MessageReceivingThread(QSslConfiguration sslConfiguration,
                                               int socketDescriptor, QObject* parent)
    : QThread(parent), sslConfiguration(sslConfiguration), socketDescriptor(socketDescriptor), messageSize(0) {
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
}

void MessageReceivingThread::run() {
    socket = new QSslSocket();
    socket->setPeerVerifyMode(QSslSocket::VerifyNone);;

    socket->setSslConfiguration(sslConfiguration);

    qDebug() << socket->privateKey().algorithm();

    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(quit()));

    connect(socket, SIGNAL(modeChanged(QSslSocket::SslMode)),
            this, SLOT(onModeChanged(QSslSocket::SslMode)), Qt::DirectConnection);
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onStateChanged(QAbstractSocket::SocketState)), Qt::DirectConnection);

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        emit error(socket->errorString());
        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readMessage()), Qt::DirectConnection);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(processSocketError(QAbstractSocket::SocketError)), Qt::DirectConnection);

    socket->startServerEncryption();

    exec();
}

void MessageReceivingThread::processSocketError(QAbstractSocket::SocketError) {
    if (socket->error() != QAbstractSocket::RemoteHostClosedError) {
        emit error(socket->errorString());
    }
}

void MessageReceivingThread::readMessage() {
//    QDataStream in(socket);

//    if (messageSize == 0) {
//        if (socket->bytesAvailable() < (int)sizeof(quint16)) {
//            return;
//        }

//        in >> messageSize;
//    }

//    if (socket->bytesAvailable() < messageSize) {
//        return;
//    }

//    QString message;

//    in >> message;

//    emit messageReceived(message);

    socket->waitForDisconnected();
    exit();
}


void MessageReceivingThread::onStateChanged(QAbstractSocket::SocketState state) {
    qDebug() << "state changed to " << state;
}

void MessageReceivingThread::onModeChanged(QSslSocket::SslMode mode) {
    qDebug() << "mode changed to " << mode;
}
