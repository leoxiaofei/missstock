#ifndef MISSSTOCKQUERY_H
#define MISSSTOCKQUERY_H

#include <QWidget>
#include "ui_missstockquery.h"

class QSqlQuery;

class MissStockQuery : public QWidget
{
    Q_OBJECT

public:
    MissStockQuery(QWidget *parent = 0);
    ~MissStockQuery();

    bool queryStatistic(QSqlQuery& sqlQuery);
    bool queryDetail(QSqlQuery& sqlQuery);

public slots:
    void s_btnQuery();
    void s_queryTypeChange(int nIndex);

signals:
    void sg_QueryDB(const QSqlQuery& sqlQuery, const std::vector<QString>& vecHeader);

private:
    Ui::MissStockQuery ui;
    std::vector<QString> m_vecHeader;
};

#endif // MISSSTOCKQUERY_H
