#include "StdAfx.h"
#include "misshistoryquery.h"
#include <QSqlQuery>

MissHistoryQuery::MissHistoryQuery(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.dateValue->setDate(QDate::currentDate());
    m_vecHeader.push_back(tr("序列号"));
    m_vecHeader.push_back(tr("时间"));
    m_vecHeader.push_back(tr("行为"));
}

MissHistoryQuery::~MissHistoryQuery()
{

}

void MissHistoryQuery::s_btnQuery()
{
    QSqlQuery sql;
    QString strValue = ui.edtValue->text().trimmed();
    if (strValue.isEmpty())
    {
    sql.prepare(
        "SELECT ob.pdi_Serial AS Serial, DATETIME(ob.ob_PutDate) AS PutDate,"
        "obe.obe_Name AS Reason FROM OutBook AS ob LEFT JOIN OutBookEnum "
        "AS obe ON ob.ob_Reason = obe.ob_Reason WHERE DATE(ob.ob_PutDate) "
        + ui.cboxOperator->currentText() + " DATE(:PutDate1) "
        "UNION "
        "SELECT pb.pdi_Serial AS Serial, DATETIME(pb.pb_PutDate) AS PutDate,"
        "pbe.pbe_Name AS Reason FROM PutBook AS pb LEFT JOIN PutBookEnum "
        "AS pbe ON pb.pb_Reason = pbe.pb_Reason WHERE DATE(pb.pb_PutDate) "
        + ui.cboxOperator->currentText() + " DATE(:PutDate2) "
        "ORDER BY PutDate;"
        );
    }
    else
    {
        sql.prepare(
            "SELECT ob.pdi_Serial AS Serial, DATETIME(ob.ob_PutDate) AS PutDate,"
            "obe.obe_Name AS Reason FROM OutBook AS ob LEFT JOIN OutBookEnum "
            "AS obe ON ob.ob_Reason = obe.ob_Reason WHERE DATE(ob.ob_PutDate) "
            + ui.cboxOperator->currentText() + " DATE(:PutDate1) AND "
            "ob.pdi_Serial = :pdi_Serial1 "
            "UNION "
            "SELECT pb.pdi_Serial AS Serial, DATETIME(pb.pb_PutDate) AS PutDate,"
            "pbe.pbe_Name AS Reason FROM PutBook AS pb LEFT JOIN PutBookEnum "
            "AS pbe ON pb.pb_Reason = pbe.pb_Reason WHERE DATE(pb.pb_PutDate) "
            + ui.cboxOperator->currentText() + " DATE(:PutDate2) AND "
            "pb.pdi_Serial = :pdi_Serial2 "
            "ORDER BY PutDate;"
            );
        sql.bindValue(":pdi_Serial1",strValue);
        sql.bindValue(":pdi_Serial2",strValue);
    }
    sql.bindValue(":PutDate1",ui.dateValue->date());
    sql.bindValue(":PutDate2",ui.dateValue->date());
    if(sql.exec())
    {
        emit sg_QueryDB(sql,m_vecHeader);
    }
    else
    {
        qDebug() << sql.executedQuery();
    }
}
