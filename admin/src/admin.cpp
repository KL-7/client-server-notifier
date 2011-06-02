#include <QtNetwork>
#include <QMessageBox>
#include <QtDebug>

#include "admin.h"


const int Admin::SERVER_WAIT_TIMEOUT = 2000;


Admin::Admin(QWidget *parent) : QDialog(parent), socket(0) {
    setupUi(this);

    connect(toggleConnectionPushButton, SIGNAL(clicked()), this, SLOT(toggleConnection()));
    connect(serverHostLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(sendPushButton, SIGNAL(clicked()), this, SLOT(sendMessage()));
}

void Admin::updateUi() {
    serverHostLineEdit->setDisabled(isConnected());
    serverPortSpinBox->setDisabled(isConnected());
    messageGroupBox->setEnabled(isConnected());
    toggleConnectionPushButton->setText(isConnected() ? tr("Dis&connect") : tr("&Connect"));
}

void Admin::toggleConnection() {
    if (isConnected()) {
        disconnectFromServer();
    } else {
        connectToServer();
    }

    updateUi();
}

bool Admin::isConnected() {
    return socket && socket->state() == QAbstractSocket::ConnectedState;
}

void Admin::connectToServer() {
    if (!socket) {
        socket = new QTcpSocket;
        connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(onError(QAbstractSocket::SocketError)));
    }

    socket->connectToHost(serverHostLineEdit->text(), serverPortSpinBox->value());

    if (socket->waitForConnected(SERVER_WAIT_TIMEOUT)) {
        QMessageBox::information(this, tr("Success"), tr("Successfully connected to server"));
    }
}

void Admin::onStateChanged(QAbstractSocket::SocketState state) {
    qDebug() << "state changed to " << state;
    updateUi();
}

void Admin::disconnectFromServer() {
    socket->disconnectFromHost();
}

void Admin::sendMessage() {
    if (!isConnected()) {
        QMessageBox::critical(this, tr("Connection error"), tr("Failed to send message: no connection to server"));
        return;
    }

    if (socket->waitForConnected(SERVER_WAIT_TIMEOUT)) {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        out << (quint16)0;
        out << (quint16)clientIdSpinBox->value();
        out << (quint16)clientPortSpinBox->value();
        out << clientHostLineEdit->text();
        out << messageBodyTextEdit->toPlainText();

        qDebug() << (quint16)clientIdSpinBox->value();

        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

        socket->write(block);

        QMessageBox::information(this, tr("Success"), tr("Message was sent delivered to server"));
    } else {
        QMessageBox::critical(this, tr("Connection error"), socket->errorString());
    }
}

void Admin::onError(QAbstractSocket::SocketError) {
    QMessageBox::critical(this, tr("Connection error"), socket->errorString());
    updateUi();
}
