#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>

#include "ui_client.h"

class ListeningTcpServer;
class QTcpSocket;
class QHostAddress;

class Client : public QDialog, private Ui::Client {
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);

private:
    enum LogMessageType { SYS, MSG, ERR };

private slots:
    void updateUi();
    void toggleSocketListening();
    void startSocketListening();
    void stopSocketListening();
    void logMessage(QString message);
    void logError(QString error);
    void log(QString message, LogMessageType type = MSG);

private:
    QHostAddress host();
    quint16 port();
    bool isListening();
    QString listeningStartedMessage();
    QString logMessageTypeToQString(LogMessageType logMessageType);

    ListeningTcpServer* listeningServer;
    QTcpSocket* serverConnection;

};

#endif // CLIENT_H
