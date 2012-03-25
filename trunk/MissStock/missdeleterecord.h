#ifndef MISSDELETERECORD_H
#define MISSDELETERECORD_H

#include <QDialog>
#include "ui_missdeleterecord.h"

class MissDeleteRecord : public QDialog
{
    Q_OBJECT

public:
    MissDeleteRecord(QWidget *parent = 0);
    ~MissDeleteRecord();

protected:
    bool checkForm(QString& strReason);

public slots:
    virtual void accept();
    void s_queryData();
    
private:
    Ui::MissDeleteRecord ui;
    bool m_bFind;
};

#endif // MISSDELETERECORD_H
