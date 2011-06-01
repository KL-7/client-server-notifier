#ifndef ADDMESSAGEDIALOG_H
#define ADDMESSAGEDIALOG_H

#include "ui_addmessagedialog.h"
#include "message.h"

class AddMessageDialog : public QDialog, private Ui::AddMessageDialog {
    Q_OBJECT

public:
    explicit AddMessageDialog(QWidget *parent = 0);

signals:
    void addMessage(Message);

private slots:
    void updateUi();
    void addMessage();

};

#endif // ADDMESSAGEDIALOG_H
