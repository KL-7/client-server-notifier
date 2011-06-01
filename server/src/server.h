#ifndef SERVER_H
#define SERVER_H

#include "ui_server.h"


class Server : public QDialog, private Ui::Server {
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);

private slots:
    void updateUi();
    void sendMessage();
    void sendMessage(QString host, quint16 port, int clientId, QString message);

private:
    static const int CLIENT_WAIT_TIMEOUT;
};

#endif // SERVER_H
