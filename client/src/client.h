#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>

#include "ui_client.h"

class ServerConnectionTcpServer;
class QTcpSocket;
class QHostAddress;

class Client : public QDialog, private Ui::Client {
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);

private slots:
    void updateUi();
    void toggleSocketListening();
    void startSocketListening();
    void stopSocketListening();
    void logMessage(QString message);
    void logError(QString error);

private:
    enum LogMessageType { SYS, MSG, ERR };

    ServerConnectionTcpServer* listeningServer;
    QTcpSocket* serverConnection;

    QHostAddress host();
    quint16 port();
    quint16 clientId();
    bool isListening();
    QString listeningStartedMessage();
    QString logMessageTypeToQString(LogMessageType logMessageType);
    void log(QString message, LogMessageType type = MSG);

};

#endif // CLIENT_H
