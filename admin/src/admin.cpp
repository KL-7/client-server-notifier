#include <QtNetwork>
#include <QMessageBox>
#include <QtDebug>

#include "admin.h"


Admin::Admin(QWidget *parent) : QDialog(parent), socket(0) {
    setupUi(this);

    connect(toggleConnectionPushButton, SIGNAL(clicked()), this, SLOT(toggleConnection()));
    connect(serverHostLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
}

void Admin::updateUi() {

}

void Admin::toggleConnection() {
    if (isConnected()) {
        disconnectFromServer();
    } else {
        connectToServer();
    }
}

bool Admin::isConnected() {
    return socket && socket->state() == QAbstractSocket::ConnectedState;
}

void Admin::connectToServer() {
    if (!socket) {
        socket = new QSslSocket;
        socket->setPeerVerifyMode(QSslSocket::VerifyNone);
//        socket->setCiphers("ADH-RC4-MD5");
    }

    connect(socket, SIGNAL(modeChanged(QSslSocket::SslMode)), this, SLOT(onModeChanged(QSslSocket::SslMode)));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onStateChanged(QAbstractSocket::SocketState)));

    socket->connectToHostEncrypted(serverHostLineEdit->text(), serverPortSpinBox->value());

    if (socket->waitForEncrypted()) {
        QMessageBox::information(this, tr("Success"), tr("Successfully connected to server"));
    } else {
        QMessageBox::critical(this, tr("Connection error"), socket->errorString());
    }

    socket->disconnectFromHost();
}

void Admin::onStateChanged(QAbstractSocket::SocketState state) {
    qDebug() << "state changed to " << state;
}

void Admin::onModeChanged(QSslSocket::SslMode mode) {
    qDebug() << "mode changed to " << mode;
}

void Admin::disconnectFromServer() {

}
