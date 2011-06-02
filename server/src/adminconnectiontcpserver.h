#ifndef ADMINCONNECTIONTCPSERVER_H
#define ADMINCONNECTIONTCPSERVER_H

#include <QTcpServer>

class AdminConnectionTcpServer : public QTcpServer {
    Q_OBJECT

signals:
    void messageReceived(QString message);
    void error(QString socketError);

public:
    AdminConnectionTcpServer(QObject* parent = 0);

protected:
    void incomingConnection(int socketDescriptor);

};

#endif // ADMINCONNECTIONTCPSERVER_H
