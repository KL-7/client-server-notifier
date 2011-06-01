#include <QtNetwork>
#include <QMessageBox>
#include <QtDebug>

#include "server.h"
#include "sendmessagethread.h"

const int Server::CLIENT_WAIT_TIMEOUT = 2000;

Server::Server(QWidget *parent) : QDialog(parent) {
    setupUi(this);

    clientIdLineEdit->setValidator(new QRegExpValidator(QRegExp(QString("\\d*")), this));
    messageIdLineEdit->setValidator(new QRegExpValidator(QRegExp(QString("\\d*")), this));

    connect(clientHostLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(clientIdLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(sendPushButton, SIGNAL(clicked()), this, SLOT(sendMessage()));

    updateUi();
}

void Server::updateUi() {
    sendPushButton->setEnabled(!clientHostLineEdit->text().isEmpty() && !clientIdLineEdit->text().isEmpty());
}

void Server::sendMessage() {
    Message message(
        messageIdLineEdit->text().toInt(),
        clientIdLineEdit->text().toInt(),
        clientHostLineEdit->text(),
        clientPortSpinBox->value(),
        messageTextEdit->toPlainText()
    );

    sendMessage(message);
}

void Server::sendMessage(Message message) {
    SendMessageThread* thread = new SendMessageThread(message, CLIENT_WAIT_TIMEOUT, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(error(QString)), this, SLOT(onError(QString)));
    connect(thread, SIGNAL(messageDelivered(int)), this, SLOT(onMessageDelivered(int)));
    thread->start();
}

void Server::onError(QString error) {
    log(error, ERR);
}

void Server::onMessageDelivered(int messageId) {
    log(tr("Message %1 successfully delivered").arg(messageId));
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
