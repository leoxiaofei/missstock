#include "StdAfx.h"
#include "missloanquery.h"
#include <QSqlQuery>
#include "MissGlobal.h"


MissLoanQuery::MissLoanQuery(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.dateValue->setDateTime(QDateTime::currentDateTime());
    showDate(false);
    m_vecHeader.push_back(tr("产品型号"));
    m_vecHeader.push_back(tr("序列号"));
    m_vecHeader.push_back(tr("外借日期"));
    m_vecHeader.push_back(tr("外借人"));
    m_vecHeader.push_back(tr("最终用户"));
    m_vecHeader.push_back(tr("预计归还日期"));
    m_vecHeader.push_back(tr("当前状态"));
}

MissLoanQuery::~MissLoanQuery()
{

}

void MissLoanQuery::s_conditionChange( int nIndex )
{
    switch(nIndex)
    {
    case 2:
    case 5:
        showDate(true);
        break;
    default:
        showDate(false);
        break;
    }
}

void MissLoanQuery::s_btnQuery()
{
    int nIndex;
    QString strValue = ui.edtValue->text().trimmed();
    QSqlQuery sql;
    QString strState;
    nIndex = ui.cBoxState->currentIndex();
    switch(nIndex)
    {
    case 3:
        strState = tr("1 = 1 ");
        break;
    default:
        strState = tr("ld.ld_State = %1 ").arg(nIndex);
        break;
    }
    nIndex = ui.cboxCondition->currentIndex();
    if (nIndex != 2 && nIndex != 5 && strValue.isEmpty())
    {
        sql.prepare(MissGlobal::strQueryLoan + strState);
    }
    else
    {
        switch(nIndex)
        {
        case 0:
            sql.prepare(MissGlobal::strQueryLoan + strState + "AND pi.pi_Name = :pi_Name");
            sql.bindValue(":pi_Name",strValue);
            break;
        case 1:
            sql.prepare(MissGlobal::strQueryLoan + strState + "AND ld.pdi_Serial = :pdi_Serial");
            sql.bindValue(":pdi_Serial",strValue);
            break;
        case 2:
            strValue = ui.cboxOperator->currentText();
            sql.prepare(MissGlobal::strQueryLoan + strState + "AND DATE(ld.ld_LoanDate) " + strValue+ " DATE(:ld_LoanDate)");
            sql.bindValue(":ld_LoanDate",ui.dateValue->date());
            break;
        case 3:
            sql.prepare(MissGlobal::strQueryLoan + strState + "AND ei.ei_Name = :ei_Name");
            sql.bindValue(":ei_Name",strValue);
            break;
        case 4:
            sql.prepare(MissGlobal::strQueryLoan + strState + "AND fi.fi_Name = :fi_Name");
            sql.bindValue(":fi_Name",strValue);
            break;
        case 5:
            strValue = ui.cboxOperator->currentText();
            sql.prepare(MissGlobal::strQueryLoan + strState + "AND DATE(ld.ld_PlanDate) " + strValue + " DATE(:ld_PlanDate)");
            sql.bindValue(":ld_PlanDate",ui.dateValue->date());
            break;
        default:
            break;
        }
    }

    if(sql.exec())
    {
        emit sg_QueryDB(sql,m_vecHeader);
    }
    else
    {
        qDebug() << sql.executedQuery();
    }
}

void MissLoanQuery::showDate( bool bValue )
{
    ui.edtValue->setVisible(!bValue);
    ui.dateValue->setVisible(bValue);
    ui.cboxOperator->setVisible(bValue);
}
