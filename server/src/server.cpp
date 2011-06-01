#include <QtNetwork>
#include <QMessageBox>
#include <QtDebug>
#include <Qt/QtSql>

#include "server.h"
#include "sendmessagethread.h"
#include "addmessagedialog.h"


const int Server::CLIENT_WAIT_TIMEOUT = 2000;
const int Server::MESSAGES_DELIVERY_BUNCH_SIZE = 10;
const int Server::MESSAGES_DELIVERY_INTERVAL = 10000;
const QString Server::DATABASE_NAME = QString("messages.sqlite");

Server::Server(QWidget *parent) : QDialog(parent), addMessageDialog(0) {
    initUi();
    initDB();

    deliveryTimer = new QTimer(this);
    connect(deliveryTimer, SIGNAL(timeout()), this, SLOT(deliverMessagesBunch()));

    updateUi();
}

void Server::initDB() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString path = QApplication::applicationDirPath();
    path.append(QDir::separator()).append(DATABASE_NAME);
    path = QDir::toNativeSeparators(path);
    db.setDatabaseName(path);

    if (db.open()) {
        log(tr("DB connection established"));
    } else {
        log(tr("Failed to open DB"), ERR);
    }
}

void Server::initUi() {
    setupUi(this);

    connect(startStopPushButton, SIGNAL(clicked()), this, SLOT(toggleDelivery()));
    connect(addMessagePushButton, SIGNAL(clicked()), this, SLOT(showAddMessageDialog()));
}

void Server::toggleDelivery() {
    if (deliveryTimer->isActive()) {
        deliveryTimer->stop();
        startStopPushButton->setText(tr("&Start"));
    } else {
        deliverMessagesBunch();
        deliveryTimer->start(MESSAGES_DELIVERY_INTERVAL);
        startStopPushButton->setText(tr("&Stop"));
    }
}

void Server::showAddMessageDialog() {
    if (!addMessageDialog) {
        addMessageDialog = new AddMessageDialog(this);
        connect(addMessageDialog, SIGNAL(addMessage(Message)), this, SLOT(addMessage(Message)));
    }

    addMessageDialog->show();
    addMessageDialog->raise();
    addMessageDialog->activateWindow();
}

void Server::updateUi() {
    pendingMessagesCountLabel->setText(QString::number(Message::count()));
}

void Server::deliverMessagesBunch() {
    qDebug() << "delivering";
    foreach(Message message, Message::oldest(MESSAGES_DELIVERY_BUNCH_SIZE)) {
        sendMessage(message);
    }
    qDebug() << Message::lastError();
}

void Server::addMessage(Message message) {
    message.save();
    if (message.lastError().isEmpty()) {
        log(tr("Message with id %1 successfully saved").arg(message.id));
    } else {
        log(tr("Failed to save message into DB"), ERR);
        log(message.lastError(), ERR);
    }
    updateUi();
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
    Message* message = Message::byId(messageId);
    if (message) {
        message->destroy();
    }
    log(tr("Message %1 successfully delivered").arg(messageId));

    updateUi();
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
