#include <QTcpSocket>

#include "messagereceivingthread.h"


MessageReceivingThread::MessageReceivingThread(int socketDescriptor, int clientId, QObject* parent)
    : QThread(parent), clientId(clientId), socketDescriptor(socketDescriptor), messageSize(0) {
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
}

void MessageReceivingThread::run() {
    tcpSocket = new QTcpSocket();
    connect(tcpSocket, SIGNAL(disconnected()), tcpSocket, SLOT(deleteLater()));

    if (!tcpSocket->setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket->errorString());
        return;
    }

    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage()), Qt::DirectConnection);
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(processSocketError(QAbstractSocket::SocketError)), Qt::DirectConnection);

    exec();
}

void MessageReceivingThread::processSocketError(QAbstractSocket::SocketError) {
    emit error(tcpSocket->errorString());
}

void MessageReceivingThread::readMessage() {
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_6);

    if (messageSize == 0) {
        if (tcpSocket->bytesAvailable() < (int)sizeof(quint16)) {
            return;
        }

        in >> messageSize;
    }

    if (tcpSocket->bytesAvailable() < messageSize) {
        return;
    }

    int messageClientId;
    in >> messageClientId;

    if (messageClientId != clientId) {
        emit error(tr("Wrong client id. Message ignored."));
    } else {
        QString message;
        in >> message;

        emit messageReceived(message);
    }

    tcpSocket->disconnectFromHost();
    exit();
}
