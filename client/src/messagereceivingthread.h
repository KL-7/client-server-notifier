#ifndef MESSAGERECEIVINGTHREAD_H
#define MESSAGERECEIVINGTHREAD_H

#include <QThread>
#include <QSslSocket>


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
    void onModeChanged(QSslSocket::SslMode mode);
    void onStateChanged(QAbstractSocket::SocketState state);

private:
    quint16 clientId;
    int socketDescriptor;
    quint16 messageSize;
    QSslSocket* socket;

};

#endif // MESSAGERECEIVINGTHREAD_H
