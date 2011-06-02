#ifndef MESSAGERECEIVINGTHREAD_H
#define MESSAGERECEIVINGTHREAD_H

#include <QThread>
#include <QtNetwork>
#include "message.h"


Q_DECLARE_METATYPE(QAbstractSocket::SocketError)

class MessageReceivingThread : public QThread {
    Q_OBJECT

signals:
    void messageReceived(Message message);
    void error(QString error);

public:
    MessageReceivingThread(QSslConfiguration sslConfiguration, int socketDescriptor, QObject* parent = 0);

protected:
    void run();

private slots:
    void readMessage();
    void processSocketError(QAbstractSocket::SocketError);
    void onModeChanged(QSslSocket::SslMode mode);
    void onStateChanged(QAbstractSocket::SocketState state);

private:
    QSslConfiguration sslConfiguration;
    int socketDescriptor;
    quint16 messageSize;
    QSslSocket* socket;

};

#endif // MESSAGERECEIVINGTHREAD_H
