#ifndef MISSHISTORYQUERY_H
#define MISSHISTORYQUERY_H

#include <QWidget>
#include "ui_misshistoryquery.h"

class QSqlQuery;

class MissHistoryQuery : public QWidget
{
    Q_OBJECT

public:
    MissHistoryQuery(QWidget *parent = 0);
    ~MissHistoryQuery();

public slots:
    void s_btnQuery();

signals:
    void sg_QueryDB(const QSqlQuery& sqlQuery, const std::vector<QString>& vecHeader);

private:
    Ui::MissHistoryQuery ui;
    std::vector<QString> m_vecHeader;
};

#endif // MISSHISTORYQUERY_H
