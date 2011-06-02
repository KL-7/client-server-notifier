#include <QtNetwork>

#include "messagesendingthread.h"


MessageSendingThread::MessageSendingThread(Message message, int timeout, QObject* parent)
    : QThread(parent), message(message), timeout(timeout) {
}

void MessageSendingThread::run() {
    tcpSocket = new QTcpSocket();
    tcpSocket->connectToHost(message.host, message.port);

    connect(tcpSocket, SIGNAL(disconnected()), tcpSocket, SLOT(deleteLater()));

    if (tcpSocket->waitForConnected(timeout)) {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        out << (quint16)0;
        out << message.clientId;
        out << message.body;

        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

        tcpSocket->write(block);

        if(tcpSocket->waitForReadyRead(timeout)) {
            QDataStream in(tcpSocket);
            quint8 res;
            in >> res;
            if (res == 1) {
                emit messageDelivered(message.id);
            } else {
                emit error(tr("Failed to deliver message to #%1 (wrong client id or host)").arg(message.clientId));
            }
        } else {
            emit error(tr("Failed to get response from client #%1 in %2 sec").arg(message.clientId).arg(timeout / 1000));
        }

        tcpSocket->disconnectFromHost();
    } else {
        emit error(tr("Failed to connect to %1:%2 in %3 sec").arg(message.host).arg(message.port).arg(timeout / 1000));
    }
}
