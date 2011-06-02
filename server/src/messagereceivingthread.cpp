#include <QTcpSocket>
#include <QtDebug>

#include "messagereceivingthread.h"


MessageReceivingThread::MessageReceivingThread(int socketDescriptor, QObject* parent)
    : QThread(parent), socketDescriptor(socketDescriptor), messageSize(0) {
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
}

void MessageReceivingThread::run() {
    adminConnection = new QTcpSocket();
    connect(adminConnection, SIGNAL(disconnected()), adminConnection, SLOT(deleteLater()));
    connect(adminConnection, SIGNAL(disconnected()), this, SLOT(quit()));

    if (!adminConnection->setSocketDescriptor(socketDescriptor)) {
        emit error(adminConnection->errorString());
        return;
    }

    connect(adminConnection, SIGNAL(readyRead()), this, SLOT(readMessage()), Qt::DirectConnection);
    connect(adminConnection, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(processSocketError(QAbstractSocket::SocketError)), Qt::DirectConnection);

    exec();
}

void MessageReceivingThread::processSocketError(QAbstractSocket::SocketError) {
    if (adminConnection->error() != QAbstractSocket::RemoteHostClosedError) {
        emit error(adminConnection->errorString());
    }
}

void MessageReceivingThread::readMessage() {
    QDataStream in(adminConnection);

    if (messageSize == 0) {
        if (adminConnection->bytesAvailable() < (int)sizeof(quint16)) {
            return;
        }

        in >> messageSize;
    }

    if (adminConnection->bytesAvailable() < messageSize) {
        return;
    }

    QString message;

    in >> message;

    emit messageReceived(message);

    adminConnection->waitForDisconnected();
    exit();
}
