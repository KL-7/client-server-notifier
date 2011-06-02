#ifndef SENDMESSAGETHREAD_H
#define SENDMESSAGETHREAD_H

#include <QThread>
#include <QTcpSocket>

#include "message.h"


class MessageSendingThread : public QThread {
    Q_OBJECT

public:
    MessageSendingThread(Message message, int timeout, QObject* parent = 0);

signals:
    void error(QString error);
    void messageDelivered(int messageId);

private slots:
    void onStateChanged(QAbstractSocket::SocketState state);

protected:
    void run();

private:
    QTcpSocket* socket;

    Message message;
    int timeout;

};

#endif // SENDMESSAGETHREAD_H
