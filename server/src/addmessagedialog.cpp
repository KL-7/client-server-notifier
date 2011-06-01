#include "addmessagedialog.h"

AddMessageDialog::AddMessageDialog(QWidget *parent) : QDialog(parent) {
    setupUi(this);

    clientIdLineEdit->setValidator(new QRegExpValidator(QRegExp(QString("\\d*")), this));

    connect(clientHostLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(clientIdLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(addPushButton, SIGNAL(clicked()), this, SLOT(addMessage()));

    updateUi();
}

void AddMessageDialog::updateUi() {
    addPushButton->setEnabled(!clientHostLineEdit->text().isEmpty() && !clientIdLineEdit->text().isEmpty());
}

void AddMessageDialog::addMessage() {
    Message message(
        clientIdLineEdit->text().toInt(),
        clientHostLineEdit->text(),
        clientPortSpinBox->value(),
        messageTextEdit->toPlainText()
    );

    emit addMessage(message);
}
