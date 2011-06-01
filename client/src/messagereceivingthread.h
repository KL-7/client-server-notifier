#ifndef MESSAGERECEIVINGTHREAD_H
#define MESSAGERECEIVINGTHREAD_H

#include <QThread>
#include <QTcpSocket>


Q_DECLARE_METATYPE(QAbstractSocket::SocketError)

class MessageReceivingThread : public QThread {
    Q_OBJECT

signals:
    void messageReceived(QString message);
    void error(QString error);

public:
    MessageReceivingThread(int socketDescriptor, int clientId, QObject* parent = 0);

protected:
    void run();

private slots:
    void readMessage();
    void processSocketError(QAbstractSocket::SocketError);

private:
    int clientId;
    int socketDescriptor;
    quint16 messageSize;
    QTcpSocket* tcpSocket;

};

#endif // MESSAGERECEIVINGTHREAD_H
