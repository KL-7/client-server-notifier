#include <QtNetwork>
#include <QMessageBox>
#include <QtDebug>
#include <QtSql>

#include "server.h"
#include "messagesendingthread.h"
#include "addmessagedialog.h"


const int Server::CLIENT_WAIT_TIMEOUT = 2000;
const int Server::MESSAGES_DELIVERY_BUNCH_SIZE = 10;
const int Server::MESSAGES_DELIVERY_INTERVAL = 10000;
const QString Server::DATABASE_NAME = QString("messages.sqlite");

Server::Server(QWidget *parent) : QDialog(parent), addMessageDialog(0), adminConnectionServer(0) {
    qRegisterMetaType<Message>("Message");

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
        log(tr("Failed to open DB (%1)").arg(db.lastError().text()), ERR);
    }
}

void Server::initUi() {
    setupUi(this);

    connect(startStopDeliveryLoopPushButton, SIGNAL(clicked()), this, SLOT(toggleDelivery()));
    connect(addMessagePushButton, SIGNAL(clicked()), this, SLOT(showAddMessageDialog()));
    connect(toggleAdminConnectionPushButton, SIGNAL(clicked()), this, SLOT(toggleAdminConnection()));
    connect(hostLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(deleteAllMessagesPushButton, SIGNAL(clicked()), this, SLOT(deleteAllMessages()));
}

void Server::toggleDelivery() {
    if (deliveryTimer->isActive()) {
        deliveryTimer->stop();
        startStopDeliveryLoopPushButton->setText(tr("&Start delivery loop"));
        log(tr("Delivery loop stopped"), SYS);
    } else {
        deliverMessagesBunch();
        deliveryTimer->start(MESSAGES_DELIVERY_INTERVAL);
        startStopDeliveryLoopPushButton->setText(tr("&Stop delivery loop"));
        log(tr("Delivery loop started"), SYS);
    }

    updateUi();
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
    hostLineEdit->setDisabled(isAdminConnectionRunning());
    portSpinBox->setDisabled(isAdminConnectionRunning());
    toggleAdminConnectionPushButton->setText(
            isAdminConnectionRunning() ? tr("&Disable admin connection") : tr("&Enable admin connection"));
}

void Server::toggleAdminConnection() {
    if (isAdminConnectionRunning()) {
        closeAdminConnection();
    } else {
        openAdminConnection();
    }

    updateUi();
}

void Server::openAdminConnection() {
    if (!adminConnectionServer) {
        adminConnectionServer = new AdminConnectionTcpServer(this);
        connect(adminConnectionServer, SIGNAL(messageReceived(Message)), this, SLOT(onMessageRecieved(Message)));
        connect(adminConnectionServer, SIGNAL(error(QString)),
                this, SLOT(onError(QString)));
    } else if (isAdminConnectionRunning()) {
        adminConnectionServer->close();
    }

    if (!adminConnectionServer->listen(host(), port())) {
        log(adminConnectionServer->errorString(), ERR);
        return;
    }

    log(adminConnectionOpenedMessage(), SYS);
}

void Server::deleteAllMessages() {
    Message::destroyAll();
    updateUi();
}

void Server::closeAdminConnection() {
    if (isAdminConnectionRunning()) {
        adminConnectionServer->closeConnection();
        adminConnectionServer->close();
    }

    log(tr("Admin connection closed"), SYS);
}

QHostAddress Server::host() {
    if (autoSocketCheckBox->isChecked()) {
        return QHostAddress::Any;
    } else {
        if (hostLineEdit->text().toLower() == tr("localhost")) {
            return QHostAddress::LocalHost;
        } else {
            return QHostAddress(hostLineEdit->text());
        }
    }
}

quint16 Server::port() {
    return autoSocketCheckBox->isChecked() ? 0 : portSpinBox->value();
}

void Server::onMessageRecieved(Message message) {
    log(tr("Recieved message from admin"));
    addMessage(message);
}

bool Server::isAdminConnectionRunning() {
    return adminConnectionServer && adminConnectionServer->isListening();
}

void Server::deliverMessagesBunch() {
    foreach(Message message, Message::oldest(MESSAGES_DELIVERY_BUNCH_SIZE)) {
        sendMessage(message);
    }
}

void Server::addMessage(Message message) {
    message.save();
    if (!message.hasLastError()) {
        log(tr("Message with id %1 successfully saved").arg(message.id));
    } else {
        log(tr("Failed to save message into DB"), ERR);
        log(message.lastError(), ERR);
    }
    updateUi();
}

void Server::sendMessage(Message message) {
    MessageSendingThread* thread = new MessageSendingThread(message, CLIENT_WAIT_TIMEOUT, this);
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


QString Server::adminConnectionOpenedMessage() {
    if (isAdminConnectionRunning()) {
        QString ipAddress;
        QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
        for (int i = 0; i < ipAddressesList.size(); ++i) {
            if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address()) {
                ipAddress = ipAddressesList.at(i).toString();
                break;
            }
        }
        if (ipAddress.isEmpty()) {
            ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
        }
        return tr("Admin connection opened on %1:%2").arg(ipAddress).arg(adminConnectionServer->serverPort());
    } else {
        return "";
    }
}
