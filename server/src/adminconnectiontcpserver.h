#ifndef ADMINCONNECTIONTCPSERVER_H
#define ADMINCONNECTIONTCPSERVER_H

#include <QtNetwork>
#include "message.h"


class MessageReceivingThread;

class AdminConnectionTcpServer : public QTcpServer {
    Q_OBJECT

signals:
    void messageReceived(Message message);
    void error(QString socketError);
    void disconnect();

public:
    AdminConnectionTcpServer(QObject* parent = 0);
    void closeConnection();

protected:
    void incomingConnection(int socketDescriptor);

};

#endif // ADMINCONNECTIONTCPSERVER_H
