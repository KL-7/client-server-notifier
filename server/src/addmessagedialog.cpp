#include "addmessagedialog.h"

AddMessageDialog::AddMessageDialog(QWidget *parent) : QDialog(parent) {
    setupUi(this);

    connect(clientHostLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(clientIdSpinBox, SIGNAL(valueChanged(QString)), this, SLOT(updateUi()));
    connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(addPushButton, SIGNAL(clicked()), this, SLOT(addMessage()));

    updateUi();
}

void AddMessageDialog::updateUi() {
    addPushButton->setEnabled(!clientHostLineEdit->text().isEmpty() && !clientIdSpinBox->text().isEmpty());
}

void AddMessageDialog::addMessage() {
    Message message(
        clientIdSpinBox->text().toInt(),
        clientHostLineEdit->text(),
        clientPortSpinBox->value(),
        messageTextEdit->toPlainText()
    );

    emit addMessage(message);
}
