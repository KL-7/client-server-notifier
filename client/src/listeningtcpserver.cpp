#include "listeningtcpserver.h"
#include "messagereceivingthread.h"

ListeningTcpServer::ListeningTcpServer(int clientId, QObject* parent) : QTcpServer(parent), clientId(clientId) {
}

void ListeningTcpServer::incomingConnection(int socketDescriptor) {
    MessageReceivingThread* thread = new MessageReceivingThread(socketDescriptor, clientId, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(messageReceived(QString)), this, SIGNAL(messageReceived(QString)));
    connect(thread, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
    thread->start();
}
