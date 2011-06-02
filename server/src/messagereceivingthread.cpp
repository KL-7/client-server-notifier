#include <QtDebug>

#include "messagereceivingthread.h"


MessageReceivingThread::MessageReceivingThread(int socketDescriptor, QObject* parent)
    : QThread(parent), socketDescriptor(socketDescriptor), messageSize(0) {
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
}

void MessageReceivingThread::run() {
    socket = new QTcpSocket;

    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(quit()));
    connect(this, SIGNAL(finished()), socket, SLOT(deleteLater()));

    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onStateChanged(QAbstractSocket::SocketState)), Qt::DirectConnection);

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        emit error(socket->errorString());
        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readMessage()), Qt::DirectConnection);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(processSocketError(QAbstractSocket::SocketError)), Qt::DirectConnection);

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

    quint16 clientId;
    quint16 clientPort;
    QString messageBody;
    QString clientHost;

    in >> clientId;
    in >> clientPort;
    in >> clientHost;
    in >> messageBody;

    Message message(clientId, clientHost, clientPort, messageBody);
    emit messageReceived(message);
}

void MessageReceivingThread::disconnect() {
    QObject::disconnect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(processSocketError(QAbstractSocket::SocketError)));
    socket->disconnectFromHost();
    exit();
}

void MessageReceivingThread::onStateChanged(QAbstractSocket::SocketState state) {
    qDebug() << "state changed to " << state;
}
