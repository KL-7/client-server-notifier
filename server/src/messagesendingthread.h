#ifndef SENDMESSAGETHREAD_H
#define SENDMESSAGETHREAD_H

#include <QThread>
#include <QSslConfiguration>

#include "message.h"


class QSslSocket;

class MessageSendingThread : public QThread {
    Q_OBJECT

public:
    MessageSendingThread(Message message, QSslConfiguration sslConfiguration, int timeout, QObject* parent = 0);

signals:
    void error(QString error);
    void messageDelivered(int messageId);

private slots:
    void onStateChanged(QAbstractSocket::SocketState state);
    void onModeChanged(QSslSocket::SslMode mode);

protected:
    void run();

private:
    QSslSocket* socket;
    QSslConfiguration sslConfiguration;

    Message message;
    int timeout;

};

#endif // SENDMESSAGETHREAD_H
