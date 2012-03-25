#ifndef MISSMODIFYSHELF_H
#define MISSMODIFYSHELF_H

#include <QDialog>
#include "ui_missmodifyshelf.h"

class MissModifyShelf : public QDialog
{
    Q_OBJECT

public:
    MissModifyShelf(QWidget *parent = 0);
    ~MissModifyShelf();

protected:
    bool checkForm(QString& strReason);
    
public slots:
    virtual void accept();
    void s_cboxNameChanged(int nIndex);
    void s_queryData();

private:
    Ui::MissModifyShelf ui;
};

#endif // MISSMODIFYSHELF_H
