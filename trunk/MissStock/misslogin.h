#ifndef MISSLOGIN_H
#define MISSLOGIN_H

#include <QDialog>
#include "ui_misslogin.h"

class MissLogin : public QDialog
{
    Q_OBJECT

public:
    MissLogin(QWidget *parent = 0);
    ~MissLogin();
    bool checkForm(QString& strReason);

public slots:
    virtual void accept();

private:
    Ui::MissLogin ui;
};

#endif // MISSLOGIN_H
