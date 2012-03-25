#ifndef MISSSTOCK_H
#define MISSSTOCK_H

#include <QtGui/QMainWindow>
#include "ui_missstock.h"

class QSqlQuery;

class MissStock : public QMainWindow
{
    Q_OBJECT
    class MissStockImpl;

public:
    MissStock(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~MissStock();

public slots:
    void s_actionPutin();
    void s_actionPutout();
    void s_actionCurrentStock();
    void s_actionLoanQuery();
    void s_actionHistoryQuery();
    void s_actionAbout();
    void s_actionModifyInfo();
    void s_actionModifyShelf();
    void s_actionDeleteRecord();
    void s_actionModifyUserPass();
    void s_actionExportExcel();
    void s_optionQuery(const QSqlQuery& sqlQuery, const std::vector<QString>& vecHeader);
    void s_copyModelData(QModelIndex index);

protected:
    void addQueryOption(QWidget* pOption);
    void clearQueryOption();

private:
    Ui::MissStockClass ui;
    std::tr1::shared_ptr<MissStockImpl> m_pImpl;
};

#endif // MISSSTOCK_H
