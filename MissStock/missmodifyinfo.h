#ifndef MISSMODIFYINFO_H
#define MISSMODIFYINFO_H

#include <QDialog>
#include "ui_missmodifyinfo.h"

class MissModifyInfo : public QDialog
{
    Q_OBJECT

public:
    MissModifyInfo(QWidget *parent = 0);
    ~MissModifyInfo();

public slots:
    void accept();
    void s_changeName(int nIndex);
    void s_checkNameE();
    void s_checkNameR();

protected:
    bool checkForm(QString& strReason);

private:
    Ui::MissModifyInfo ui;
};

#endif // MISSMODIFYINFO_H
