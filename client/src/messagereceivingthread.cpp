#include <QtNetwork>
#include <QtDebug>

#include "messagereceivingthread.h"


MessageReceivingThread::MessageReceivingThread(int socketDescriptor, quint16 clientId, QObject* parent)
    : QThread(parent), clientId(clientId), socketDescriptor(socketDescriptor), messageSize(0) {
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
}

void MessageReceivingThread::run() {
    socket = new QTcpSocket;

    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(quit()));

    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onStateChanged(QAbstractSocket::SocketState)));

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        emit error(socket->errorString());
        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readMessage()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(processSocketError(QAbstractSocket::SocketError)));

    exec();
}

void MessageReceivingThread::processSocketError(QAbstractSocket::SocketError) {
    if (socket->error() != QAbstractSocket::RemoteHostClosedError) {
        emit error(socket->errorString());
    }
}

void MessageReceivingThread::readMessage() {
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

    QDataStream out(socket);
    if (messageClientId != clientId) {
        out << (quint8)0;
        emit error(tr("Wrong client id %1. Message ignored.").arg(messageClientId));
    } else {
        out << (quint8)1;
        emit messageReceived(message);
    }

    socket->disconnect();
    exit();
}

void MessageReceivingThread::onStateChanged(QAbstractSocket::SocketState state) {
    qDebug() << state;
}
