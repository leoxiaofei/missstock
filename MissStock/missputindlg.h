#ifndef MISSPUTINDLG_H
#define MISSPUTINDLG_H

#include <QDialog>
#include "ui_missputindlg.h"

class MissPutinDlg : public QDialog
{
    Q_OBJECT

public:
    MissPutinDlg(QWidget *parent = 0);
    ~MissPutinDlg();

public slots:
    void s_saveAndConti();
    void s_saveAndClose();
    void s_querySerial();
    void s_queryProdName();
    void s_timeup();

protected:
    bool checkData(QString& strReason);
    bool saveData();
    bool querySerial(const QString& strSerial);
    bool existSerial(const QString& strSerial);
    bool queryProdName(const QString& strProdName);

    void showSuccess(const QString& strTips);
private:
    Ui::MissPutinDlg ui;
    QTimer* m_pTimer;
};

#endif // MISSPUTINDLG_H
