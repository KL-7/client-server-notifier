#ifndef SENDMESSAGETHREAD_H
#define SENDMESSAGETHREAD_H

#include <QThread>

#include "message.h"


class QTcpSocket;

class SendMessageThread : public QThread {
    Q_OBJECT

public:
    SendMessageThread(Message message, int timeout, QObject* parent = 0);

signals:
    void error(QString error);
    void messageDelivered(int messageId);

protected:
    void run();

private:
    Message message;
    int timeout;

    QTcpSocket* tcpSocket;

};

#endif // SENDMESSAGETHREAD_H
