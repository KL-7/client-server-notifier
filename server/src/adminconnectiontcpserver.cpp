#include "adminconnectiontcpserver.h"
#include "messagereceivingthread.h"

AdminConnectionTcpServer::AdminConnectionTcpServer(QObject* parent) : QTcpServer(parent) {
}

void AdminConnectionTcpServer::incomingConnection(int socketDescriptor) {
    MessageReceivingThread* thread = new MessageReceivingThread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(messageReceived(QString)), this, SIGNAL(messageReceived(QString)));
    connect(thread, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
    thread->start();
}
