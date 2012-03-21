#ifndef MISSLOANQUERY_H
#define MISSLOANQUERY_H

#include <QWidget>
#include "ui_missloanquery.h"

class QSqlQuery;

class MissLoanQuery : public QWidget
{
    Q_OBJECT

public:
    MissLoanQuery(QWidget *parent = 0);
    ~MissLoanQuery();

protected:
    void showDate(bool bValue);

public slots:
    void s_conditionChange(int nIndex);
    void s_btnQuery();

signals:
    void sg_QueryDB(const QSqlQuery& sqlQuery, const std::vector<QString>& vecHeader);

private:
    Ui::MissLoanQuery ui;
    std::vector<QString> m_vecHeader;
};

#endif // MISSLOANQUERY_H
