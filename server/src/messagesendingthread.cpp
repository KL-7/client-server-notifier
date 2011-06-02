#include <QtNetwork>
#include <QtDebug>
#include <QtCrypto>

#include "messagesendingthread.h"


const QByteArray MessageSendingThread::INITIALIZATION_VECTOR("initialization_vector");
const QByteArray MessageSendingThread::AES_KEY("symmetric_key");


MessageSendingThread::MessageSendingThread(Message message, int timeout, QObject *parent)
    : QThread(parent), message(message), timeout(timeout) {
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
}

void MessageSendingThread::run() {
    socket = new QTcpSocket;

    socket->connectToHost(message.host, message.port);

    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onStateChanged(QAbstractSocket::SocketState)));

    if (socket->waitForConnected(timeout)) {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        out << (quint16)0;
        out << message.clientId;
        out << encrypt(message.body);

        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

        socket->write(block);

        if(socket->waitForReadyRead(timeout)) {
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
}

void MessageSendingThread::onStateChanged(QAbstractSocket::SocketState state) {
    qDebug() << state;
}

QByteArray MessageSendingThread::encrypt(QString text) {
    QCA::Initializer init;

    QCA::InitializationVector iv(INITIALIZATION_VECTOR);
    QCA::SymmetricKey key(AES_KEY);

    QCA::Cipher cipher(QString("aes128"), QCA::Cipher::CBC,
                       QCA::Cipher::DefaultPadding, QCA::Encode,
                       key, iv);

    return cipher.process(text.toAscii()).toByteArray();
}
