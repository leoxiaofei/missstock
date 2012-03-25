#include "StdAfx.h"
#include "missdeleterecord.h"
#include <QSqlQuery>
#include "MissGlobal.h"
#include "msautouppervalidator.h"

MissDeleteRecord::MissDeleteRecord(QWidget *parent)
    : QDialog(parent),
    m_bFind(false)
{
    ui.setupUi(this);
    ui.edtName->setValidator(new MsAutoUpperValidator(ui.edtName));
}

MissDeleteRecord::~MissDeleteRecord()
{

}

void MissDeleteRecord::s_queryData()
{
    QString strName = ui.edtName->text().trimmed();
    if (strName.isEmpty())
    {
        return;
    }
    ui.pedtShowData->setPlainText("");
    m_bFind = false;
    int nValue(0);
    QSqlQuery sql;
    for (int ix = 0; MissGlobal::strSTableNames[ix] != ""; ++ix)
    {
        sql.prepare("SELECT Count(pdi_Serial) FROM "
            + MissGlobal::strSTableNames[ix] + " WHERE pdi_Serial = :pdi_Serial;");
        sql.bindValue(":pdi_Serial",strName);
        if (sql.exec())
        {
            sql.next();
            nValue = sql.value(0).toInt();
            ui.pedtShowData->insertPlainText(
                QString("%1:%2\n").arg(MissGlobal::strSTableNames[ix])
                .arg(nValue)
                );
            if (nValue != 0)
            {
                m_bFind = true;
            }
        }
    }
}

void MissDeleteRecord::accept()
{
    QString strReason;
    if (checkForm(strReason))
    {
        QString strName = ui.edtName->text().trimmed();
        if (QMessageBox::question(this, tr("确认"), 
            tr("你确认要删除产品(%1)的所有记录吗？").arg(strName),
            QMessageBox::Yes|QMessageBox::No, QMessageBox::No)
            != QMessageBox::Yes)
        {
            return;
        }
        QSqlDatabase::database().transaction();
        QSqlQuery sql;
        bool bSuccess(true);
        for (int ix = 0; MissGlobal::strSTableNames[ix] != ""; ++ix)
        {
            sql.prepare("DELETE FROM " + MissGlobal::strSTableNames[ix] + " WHERE pdi_Serial = :pdi_Serial;");
            sql.bindValue(":pdi_Serial",strName);
            if (!sql.exec())
            {
                bSuccess = false;
                break;
            }
        }

        if (bSuccess)
        {
            QSqlDatabase::database().commit();
            QDialog::accept();
        }
        else
        {
            QSqlDatabase::database().rollback();
            QMessageBox::information(this, tr("抱歉"), tr("删除失败."));
        }
    }
    else
    {
        QMessageBox::information(this,tr("表单数据问题"), strReason);
    }
    
}

bool MissDeleteRecord::checkForm( QString& strReason )
{
    bool bRet(true);
    do 
    {
        if (ui.edtName->text().trimmed().isEmpty())
        {
            strReason += tr("序列号不能为空.");
            bRet = false;
            break;
        }

        if (!m_bFind)
        {
            strReason += tr("没有找到该产品信息，无需删除.");
            bRet = false;
            break;
        }
    } while (0);
    return bRet;
}
