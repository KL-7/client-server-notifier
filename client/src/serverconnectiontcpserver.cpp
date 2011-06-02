#include "serverconnectiontcpserver.h"
#include "messagereceivingthread.h"

ServerConnectionTcpServer::ServerConnectionTcpServer(quint16 clientId, QObject* parent)
    : QTcpServer(parent), clientId(clientId) {
}

void ServerConnectionTcpServer::incomingConnection(int socketDescriptor) {
    MessageReceivingThread* thread = new MessageReceivingThread(socketDescriptor, clientId, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(messageReceived(QString)), this, SIGNAL(messageReceived(QString)));
    connect(thread, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
    thread->start();
}
