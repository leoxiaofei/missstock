#ifndef MISSMODIFYUSERPASS_H
#define MISSMODIFYUSERPASS_H

#include <QDialog>
#include "ui_missmodifyuserpass.h"

class MissModifyUserPass : public QDialog
{
    Q_OBJECT

public:
    MissModifyUserPass(QWidget *parent = 0);
    ~MissModifyUserPass();

protected:
    void queryUser();
    bool checkForm(QString& strReason);

public slots:
    virtual void accept();

private:
    Ui::MissModifyUserPass ui;
};

#endif // MISSMODIFYUSERPASS_H
