#include "StdAfx.h"
#include "missstockquery.h"
#include <QSqlQuery>

MissStockQuery::MissStockQuery(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    m_vecHeader.reserve(6);
    s_queryTypeChange(1);
}

MissStockQuery::~MissStockQuery()
{

}

void MissStockQuery::s_btnQuery()
{
    QSqlQuery sql;
    switch( ui.cBoxQueryType->currentIndex() )
    {
    case 0:
        queryStatistic(sql);
        break;
    case 1:
        queryDetail(sql);
        break;
    }

    if(sql.exec())
    {
        emit sg_QueryDB( sql , m_vecHeader);
    }
    else
    {
        qDebug()<<sql.executedQuery();
        QMessageBox::information(this,tr("抱歉"),tr("数据库查询失败，这是软件Bug，找小飞吧。"));
    }
}

void MissStockQuery::s_queryTypeChange( int nIndex )
{
    m_vecHeader.clear();
    switch(nIndex)
    {
    case 0:
        m_vecHeader.push_back(tr("产品型号"));
        m_vecHeader.push_back(tr("货架"));
        m_vecHeader.push_back(tr("实际货存"));
        m_vecHeader.push_back(tr("产品描述"));
        break;
    case 1:
        m_vecHeader.push_back(tr("产品型号"));
        m_vecHeader.push_back(tr("产品序列号"));
        m_vecHeader.push_back(tr("货架"));
        m_vecHeader.push_back(tr("入库日期"));
        m_vecHeader.push_back(tr("产品描述"));
        m_vecHeader.push_back(tr("备注"));
        break;
    }
}

bool MissStockQuery::queryStatistic( QSqlQuery& sqlQuery )
{
    QString strValue = ui.edtValue->text().trimmed();
    if (strValue.isEmpty())
    {
        sqlQuery.prepare(
            "SELECT pi.pi_Name, t1.Shelf, t1.Count, pi_Desc FROM (SELECT pdi.pi_ID AS id, "
            "GROUP_CONCAT(distinct cs_Shelf) AS Shelf, COUNT(*) AS Count FROM CurrentStock "
            "AS cs INNER JOIN ProductDetailInfo AS pdi ON cs.pdi_Serial = pdi.pdi_Serial "
            "GROUP BY pdi.pi_ID ) AS t1 LEFT JOIN ProductInfo AS pi ON pi.pi_ID = t1.id");
    }
    else
    {
        switch (ui.cboxCondition->currentIndex())
        {
        case 0:           ///产品名称(型号)
            sqlQuery.prepare(
                "SELECT pi.pi_Name, t1.Shelf, t1.Count, pi_Desc FROM (SELECT pdi.pi_ID AS id, "
                "GROUP_CONCAT(distinct cs_Shelf) AS Shelf, COUNT(*) AS Count "
                "FROM CurrentStock AS cs INNER JOIN ProductDetailInfo AS pdi "
                "ON cs.pdi_Serial = pdi.pdi_Serial GROUP BY pdi.pi_ID ) AS t1 "
                "INNER JOIN ProductInfo AS pi ON pi.pi_ID = t1.id "
                "WHERE pi.pi_Name = :pi_Name");
            sqlQuery.bindValue(":pi_Name",strValue);
            break;
        case 1:          ///序列号
            sqlQuery.prepare(
                "SELECT pi.pi_Name, t1.Shelf, t1.Count, pi_Desc FROM (SELECT pdi.pi_ID AS id, "
                "GROUP_CONCAT(distinct cs_Shelf) AS Shelf, COUNT(*) AS Count "
                "FROM CurrentStock AS cs INNER JOIN ProductDetailInfo AS pdi "
                "ON cs.pdi_Serial = pdi.pdi_Serial WHERE cs.pdi_Serial = :pdi_Serial "
                "GROUP BY pdi.pi_ID) AS t1 INNER JOIN ProductInfo AS pi ON pi.pi_ID = t1.id ");
            sqlQuery.bindValue(":pdi_Serial",strValue);
            break;
        case 2:          ///货架号
            sqlQuery.prepare(
                "SELECT pi.pi_Name, t1.Shelf, t1.Count, pi_Desc FROM (SELECT pdi.pi_ID AS id, "
                "GROUP_CONCAT(distinct cs_Shelf) AS Shelf, COUNT(*) AS Count "
                "FROM CurrentStock AS cs INNER JOIN ProductDetailInfo AS pdi "
                "ON cs.pdi_Serial = pdi.pdi_Serial WHERE cs.cs_Shelf = :cs_Shelf "
                "GROUP BY pdi.pi_ID) AS t1 INNER JOIN ProductInfo AS pi ON pi.pi_ID = t1.id ");
            sqlQuery.bindValue(":cs_Shelf",strValue);
            break;
        }
    }
    return true;
}

bool MissStockQuery::queryDetail( QSqlQuery& sqlQuery )
{
    QString strValue = ui.edtValue->text().trimmed();
    if (strValue.isEmpty())
    {
        sqlQuery.prepare(
            "SELECT pi.pi_Name,pdi.pdi_Serial,cs.cs_Shelf,DATETIME(pb.pb_PutDate),pi.pi_Desc,pdi.pdi_Desc "
            "FROM CurrentStock AS cs "
            "LEFT JOIN ProductDetailInfo AS pdi ON cs.pdi_Serial = pdi.pdi_Serial "
            "LEFT JOIN ProductInfo AS pi ON pi.pi_ID = pdi.pi_ID "
            "LEFT JOIN PutBook AS pb ON pb.pdi_Serial = pdi.pdi_Serial "
            "WHERE pb.pb_Reason = 0"
            );
    }
    else
    {
        switch (ui.cboxCondition->currentIndex())
        {
        case 0:           ///产品名称(型号)
            sqlQuery.prepare(
                "SELECT pi.pi_Name,pdi.pdi_Serial,cs.cs_Shelf,DATETIME(pb.pb_PutDate),pi.pi_Desc,pdi.pdi_Desc "
                "FROM CurrentStock AS cs "
                "LEFT JOIN ProductDetailInfo AS pdi ON cs.pdi_Serial = pdi.pdi_Serial "
                "LEFT JOIN ProductInfo AS pi ON pi.pi_ID = pdi.pi_ID "
                "LEFT JOIN PutBook AS pb ON pb.pdi_Serial = pdi.pdi_Serial "
                "WHERE pb.pb_Reason = 0 AND pi.pi_Name = :pi_Name;"
                );
            sqlQuery.bindValue(":pi_Name",strValue);
            break;
        case 1:          ///序列号
            sqlQuery.prepare(
                "SELECT pi.pi_Name,pdi.pdi_Serial,cs.cs_Shelf,DATETIME(pb.pb_PutDate),pi.pi_Desc,pdi.pdi_Desc "
                "FROM CurrentStock AS cs "
                "LEFT JOIN ProductDetailInfo AS pdi ON cs.pdi_Serial = pdi.pdi_Serial "
                "LEFT JOIN ProductInfo AS pi ON pi.pi_ID = pdi.pi_ID "
                "LEFT JOIN PutBook AS pb ON pb.pdi_Serial = pdi.pdi_Serial "
                "WHERE pb.pb_Reason = 0 AND cs.pdi_Serial = :pdi_Serial"
                );
            sqlQuery.bindValue(":pdi_Serial",strValue);
            break;
        case 2:          ///货架号
            sqlQuery.prepare(
                "SELECT pi.pi_Name,pdi.pdi_Serial,cs.cs_Shelf,DATETIME(pb.pb_PutDate),pi.pi_Desc,pdi.pdi_Desc "
                "FROM CurrentStock AS cs "
                "LEFT JOIN ProductDetailInfo AS pdi ON cs.pdi_Serial = pdi.pdi_Serial "
                "LEFT JOIN ProductInfo AS pi ON pi.pi_ID = pdi.pi_ID "
                "LEFT JOIN PutBook AS pb ON pb.pdi_Serial = pdi.pdi_Serial "
                "WHERE pb.pb_Reason = 0 AND cs.cs_Shelf = :cs_Shelf;"
                );
            sqlQuery.bindValue(":cs_Shelf",strValue);
            break;
        }
    }
    return true;
}
