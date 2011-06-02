#ifndef LISTENINGSERVER_H
#define LISTENINGSERVER_H

#include <QTcpServer>

class ServerConnectionTcpServer : public QTcpServer {
    Q_OBJECT

signals:
    void messageReceived(QString message);
    void error(QString socketError);

public:
    ServerConnectionTcpServer(quint16 clientId, QObject* parent = 0);
    void setClientId(quint16 clientId) { this->clientId = clientId; }

protected:
    void incomingConnection(int socketDescriptor);

private:
    quint16 clientId;

};

#endif // LISTENINGSERVER_H
