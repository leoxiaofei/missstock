#ifndef MISSPUTOUTDLG_H
#define MISSPUTOUTDLG_H

#include <QDialog>
#include "ui_missputoutdlg.h"

class MissPutoutDlg : public QDialog
{
    Q_OBJECT

public:
    MissPutoutDlg(QWidget *parent = 0);
    ~MissPutoutDlg();

public slots:
    void s_outReasonChanged(int nIndex);
    void s_saveAndClose();
    void s_saveAndConti();
    void s_checkSerial();
    void s_officialChange();
    void s_timeup();

protected:
    bool saveData();
    bool checkData(QString& strReason);
    bool changeOfficialData();

    bool querySerial(const QString& strSerial);
    void queryEmployeeName();
    void queryFinalUserName();

    void showSuccess(const QString& strTips);
private:
    Ui::MissPutoutDlg ui;
    QTimer*           m_pTimer;
};

#endif // MISSPUTOUTDLG_H
