#ifndef MESSAGERECEIVINGTHREAD_H
#define MESSAGERECEIVINGTHREAD_H

#include <QThread>
#include <QTcpSocket>


Q_DECLARE_METATYPE(QAbstractSocket::SocketError)

class MessageReceivingThread : public QThread {
    Q_OBJECT

signals:
    void messageReceived(QString message);
    void error(QString socketError);

public:
    MessageReceivingThread(int socketDescriptor, QObject* parent = 0);

protected:
    void run();

private slots:
    void readMessage();
    void processSocketError(QAbstractSocket::SocketError);

private:
    int socketDescriptor;
    quint16 messageSize;
    QTcpSocket* tcpSocket;

};

#endif // MESSAGERECEIVINGTHREAD_H
