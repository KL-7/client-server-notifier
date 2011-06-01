#ifndef LISTENINGSERVER_H
#define LISTENINGSERVER_H

#include <QTcpServer>

class ListeningTcpServer : public QTcpServer {
    Q_OBJECT

signals:
    void messageReceived(QString message);
    void error(QString socketError);

public:
    ListeningTcpServer(int clientId, QObject* parent = 0);
    void setClientId(int clientId) { this->clientId = clientId; }

protected:
    void incomingConnection(int socketDescriptor);

private:
    int clientId;

};

#endif // LISTENINGSERVER_H
