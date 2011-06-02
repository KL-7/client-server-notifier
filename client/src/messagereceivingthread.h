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
    MessageReceivingThread(int socketDescriptor, quint16 clientId, QObject* parent = 0);

protected:
    void run();

private slots:
    void readMessage();
    void processSocketError(QAbstractSocket::SocketError);
    void onStateChanged(QAbstractSocket::SocketState state);

private:
    quint16 clientId;
    int socketDescriptor;
    quint16 messageSize;
    QTcpSocket* socket;

};

#endif // MESSAGERECEIVINGTHREAD_H
