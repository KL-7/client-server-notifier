#include <QtNetwork>
#include <QMessageBox>

#include "server.h"

const int Server::CLIENT_WAIT_TIMEOUT = 5000;

Server::Server(QWidget *parent) : QDialog(parent) {
    setupUi(this);

    clientIdLineEdit->setValidator(new QRegExpValidator(QRegExp(QString("\\d*")), this));

    connect(clientHostLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(clientIdLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(sendPushButton, SIGNAL(clicked()), this, SLOT(sendMessage()));

    updateUi();
}

void Server::updateUi() {
    sendPushButton->setEnabled(!clientHostLineEdit->text().isEmpty() && !clientIdLineEdit->text().isEmpty());
}

void Server::sendMessage() {
    QString host = clientHostLineEdit->text();
    quint16 port = clientPortSpinBox->value();
    int clientId = clientIdLineEdit->text().toInt();
    QString message = messageTextEdit->toPlainText();

    sendMessage(host, port, clientId, message);
}

void Server::sendMessage(QString host, quint16 port, int clientId, QString message) {
    QTcpSocket* tcpSocket = new QTcpSocket(this);
    tcpSocket->connectToHost(host, port, QIODevice::WriteOnly);

    connect(tcpSocket, SIGNAL(disconnected()), tcpSocket, SLOT(deleteLater()));

    if (tcpSocket->waitForConnected(CLIENT_WAIT_TIMEOUT)) {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);

        out << (quint16)0;
        out << clientId;
        out << message;
        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

        tcpSocket->write(block);
        tcpSocket->disconnectFromHost();
    } else {
        log(tr("Failed to connect to client in %1 sec").arg(CLIENT_WAIT_TIMEOUT / 1000), ERR);
    }
}

void Server::log(QString message, LogMessageType type) {
    logTextEdit->append(tr("%1 [%2]:  %3").arg(logMessageTypeToQString(type))
                        .arg(QTime::currentTime().toString(tr("HH:mm:ss"))).arg(message));
}

QString Server::logMessageTypeToQString(LogMessageType logMessageType) {
    switch (logMessageType) {
        case SYS: return tr("SYS");
        case ERR: return tr("ERR");
        default: return tr("MSG");
    }
}
