#ifndef LISTENINGSERVER_H
#define LISTENINGSERVER_H

#include <QTcpServer>

class ListeningTcpServer : public QTcpServer {
    Q_OBJECT

signals:
    void messageReceived(QString message);
    void error(QString socketError);

public:
    ListeningTcpServer(QObject* parent = 0);

protected:
    void incomingConnection(int socketDescriptor);

};

#endif // LISTENINGSERVER_H
