#ifndef ADMIN_H
#define ADMIN_H

#include <QtNetwork>
#include "ui_admin.h"


class Admin : public QDialog, private Ui::Admin {
    Q_OBJECT

public:
    explicit Admin(QWidget *parent = 0);

private slots:
    void updateUi();
    void toggleConnection();
    void onModeChanged(QSslSocket::SslMode mode);
    void onStateChanged(QAbstractSocket::SocketState state);

private:
    QSslSocket* socket;

    bool isConnected();
    void connectToServer();
    void disconnectFromServer();

};

#endif // ADMIN_H
