#include "listeningtcpserver.h"
#include "messagereceivingthread.h"

ListeningTcpServer::ListeningTcpServer(QObject* parent) : QTcpServer(parent) {
}

void ListeningTcpServer::incomingConnection(int socketDescriptor) {
    MessageReceivingThread* thread = new MessageReceivingThread(socketDescriptor);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(messageReceived(QString)), this, SIGNAL(messageReceived(QString)));
    connect(thread, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
    thread->start();
}
