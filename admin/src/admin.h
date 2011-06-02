#ifndef ADMIN_H
#define ADMIN_H

#include "ui_admin.h"

class Admin : public QDialog, private Ui::Admin
{
    Q_OBJECT

public:
    explicit Admin(QWidget *parent = 0);
};

#endif // ADMIN_H
