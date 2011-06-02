#include <QtNetwork>
#include <QtDebug>

#include "messagesendingthread.h"


MessageSendingThread::MessageSendingThread(Message message, QSslConfiguration sslConfiguration,
                                           int timeout, QObject *parent)
    : QThread(parent), sslConfiguration(sslConfiguration), message(message), timeout(timeout) {
}

void MessageSendingThread::run() {
    qDebug() << "server: new MessageSendingThread";

    socket = new QSslSocket();
    socket->setSslConfiguration(sslConfiguration);
    socket->connectToHost(message.host, message.port);

    socket->startServerEncryption();

    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onStateChanged(QAbstractSocket::SocketState)), Qt::DirectConnection);
    connect(socket, SIGNAL(modeChanged(QSslSocket::SslMode)),
            this, SLOT(onModeChanged(QSslSocket::SslMode)), Qt::DirectConnection);

    if (socket->waitForEncrypted(timeout)) {
        qDebug() << "server: is " << (socket->isEncrypted() ? "encrypted" : "non-encrypted");

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        out << (quint16)0;
        out << message.clientId;
        out << message.body;

        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

        socket->write(block);

        if(socket->waitForReadyRead(timeout / 4)) {
            QDataStream in(socket);
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

        socket->disconnectFromHost();
    } else {
        emit error(tr("Failed to connect to %1:%2 in %3 sec").arg(message.host).arg(message.port).arg(timeout / 1000));
    }

    qDebug() << "server: closing MessageSendingThread";
}

void MessageSendingThread::onModeChanged(QSslSocket::SslMode mode) {
    qDebug() << mode;
}

void MessageSendingThread::onStateChanged(QAbstractSocket::SocketState state) {
    qDebug() << state;
}
