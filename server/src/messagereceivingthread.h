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
    MessageReceivingThread(int socketDescriptor, QObject* parent = 0);

public slots:
    void disconnect();

protected:
    void run();

private slots:
    void readMessage();
    void processSocketError(QAbstractSocket::SocketError);
    void onStateChanged(QAbstractSocket::SocketState state);

private:
    int socketDescriptor;
    quint16 messageSize;
    QTcpSocket* socket;

};

#endif // MESSAGERECEIVINGTHREAD_H
