#include "adminconnectiontcpserver.h"
#include "messagereceivingthread.h"

AdminConnectionTcpServer::AdminConnectionTcpServer(QObject* parent)
    : QTcpServer(parent) {
}

void AdminConnectionTcpServer::incomingConnection(int socketDescriptor) {
    MessageReceivingThread* thread = new MessageReceivingThread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(messageReceived(Message)), this, SIGNAL(messageReceived(Message)));
    connect(thread, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
    connect(this, SIGNAL(disconnect()), thread, SLOT(disconnect()));
    thread->start();
}

void AdminConnectionTcpServer::closeConnection() {
    emit disconnect();
}
