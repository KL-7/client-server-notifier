#include <QTcpSocket>
#include <QtDebug>

#include "messagereceivingthread.h"


MessageReceivingThread::MessageReceivingThread(int socketDescriptor, int clientId, QObject* parent)
    : QThread(parent), clientId(clientId), socketDescriptor(socketDescriptor), messageSize(0) {
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
}

void MessageReceivingThread::run() {
    tcpSocket = new QTcpSocket();
    connect(tcpSocket, SIGNAL(disconnected()), tcpSocket, SLOT(deleteLater()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(quit()));

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
    if (tcpSocket->error() != QAbstractSocket::RemoteHostClosedError) {
        emit error(tcpSocket->errorString());
    }
}

void MessageReceivingThread::readMessage() {
    QDataStream in(tcpSocket);

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
    QString message;

    in >> messageClientId;
    in >> message;

    QDataStream out(tcpSocket);
    if (messageClientId != clientId) {
        out << (quint8)0;
        emit error(tr("Wrong client id %1. Message ignored.").arg(messageClientId));
    } else {
        out << (quint8)1;
        emit messageReceived(message);
    }

    tcpSocket->waitForDisconnected();
    exit();
}
