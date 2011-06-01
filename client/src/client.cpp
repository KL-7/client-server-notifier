#include <QtGui>
#include <QtNetwork>

#include "client.h"
#include "listeningtcpserver.h"


Client::Client(QWidget *parent) : QDialog(parent), listeningServer(0) {
    setupUi(this);

    clientIdLineEdit->setValidator(new QRegExpValidator(QRegExp(QString("\\d*")), this));

    connect(autoSocketCheckBox, SIGNAL(clicked()), this, SLOT(updateUi()));
    connect(hostLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(toggleSocketListeningPushButton, SIGNAL(clicked()), this, SLOT(toggleSocketListening()));

    updateUi();
}

void Client::updateUi() {
    hostLineEdit->setDisabled(isListening() || autoSocketCheckBox->isChecked());
    portSpinBox->setDisabled(isListening() || autoSocketCheckBox->isChecked());

    clientIdLineEdit->setDisabled(isListening());
    autoSocketCheckBox->setEnabled(!isListening());

    toggleSocketListeningPushButton->setDisabled(!isListening() && !autoSocketCheckBox->isChecked()
                                                 && (hostLineEdit->text().isEmpty()
                                                     || clientIdLineEdit->text().isEmpty()));

    toggleSocketListeningPushButton->setText(isListening() ? tr("&Stop") : tr("&Start"));
}

void Client::toggleSocketListening() {
    if (!isListening()) {
        startSocketListening();
    } else {
        stopSocketListening();
    }

    updateUi();
}

void Client::startSocketListening() {
    if (!listeningServer) {
        listeningServer = new ListeningTcpServer(clientId(), this);
        connect(listeningServer, SIGNAL(messageReceived(QString)), this, SLOT(logMessage(QString)));
        connect(listeningServer, SIGNAL(error(QString)),
                this, SLOT(logError(QString)));
    } else {
        if (isListening()) {
            listeningServer->close();
        }
    }

    if (!listeningServer->listen(host(), port())) {
        log(listeningServer->errorString(), ERR);
        return;
    }

    log(listeningStartedMessage(), SYS);
}

QHostAddress Client::host() {
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

quint16 Client::port() {
    return autoSocketCheckBox->isChecked() ? 0 : portSpinBox->value();
}

int Client::clientId() {
    return clientIdLineEdit->text().toInt();
}

void Client::stopSocketListening() {
    if (isListening()) {
        listeningServer->close();
    }

    log(tr("Stopped listening"), SYS);
}

void Client::logMessage(QString message) {
    log(message, MSG);
}

void Client::logError(QString error) {
    log(error, ERR);
}

QString Client::listeningStartedMessage() {
    if (isListening()) {
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
        return tr("Started listening to port %1 on %2").arg(listeningServer->serverPort()).arg(ipAddress);
    } else {
        return "";
    }
}

bool Client::isListening() {
    return listeningServer && listeningServer->isListening();
}

void Client::log(QString message, LogMessageType type) {
    logTextEdit->append(tr("%1 [%2]:  %3").arg(logMessageTypeToQString(type))
                        .arg(QTime::currentTime().toString(tr("HH:mm:ss"))).arg(message));
}

QString Client::logMessageTypeToQString(LogMessageType logMessageType) {
    switch (logMessageType) {
        case SYS: return tr("SYS");
        case ERR: return tr("ERR");
        default: return tr("MSG");
    }
}
