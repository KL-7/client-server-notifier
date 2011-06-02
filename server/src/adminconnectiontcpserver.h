#ifndef ADMINCONNECTIONTCPSERVER_H
#define ADMINCONNECTIONTCPSERVER_H

#include <QtNetwork>
#include "message.h"


class AdminConnectionTcpServer : public QTcpServer {
    Q_OBJECT

signals:
    void messageReceived(Message message);
    void error(QString socketError);

public:
    AdminConnectionTcpServer(QSslConfiguration sslConfiguration, QObject* parent = 0);

protected:
    void incomingConnection(int socketDescriptor);

private:
    QSslConfiguration sslConfiguration;

};

#endif // ADMINCONNECTIONTCPSERVER_H
