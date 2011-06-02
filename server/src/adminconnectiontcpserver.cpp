#include "adminconnectiontcpserver.h"
#include "messagereceivingthread.h"

AdminConnectionTcpServer::AdminConnectionTcpServer(QSslConfiguration sslConfiguration, QObject* parent)
    : QTcpServer(parent), sslConfiguration(sslConfiguration) {
}

void AdminConnectionTcpServer::incomingConnection(int socketDescriptor) {
    MessageReceivingThread* thread = new MessageReceivingThread(sslConfiguration, socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(messageReceived(Message)), this, SIGNAL(messageReceived(Message)));
    connect(thread, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
    thread->start();
}
