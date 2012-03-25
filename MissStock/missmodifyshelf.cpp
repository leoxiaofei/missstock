#include "StdAfx.h"
#include "missmodifyshelf.h"
#include <QSqlQuery>
#include "msautouppervalidator.h"

MissModifyShelf::MissModifyShelf(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    ui.edtName->setValidator(new MsAutoUpperValidator(ui.edtName));
    ui.edtModifyShelf->setValidator(new MsAutoUpperValidator(ui.edtModifyShelf));
    s_cboxNameChanged(ui.cBoxName->currentIndex());
}

MissModifyShelf::~MissModifyShelf()
{

}

void MissModifyShelf::s_cboxNameChanged( int nIndex )
{
    switch(nIndex)
    {
    case 0:
        ui.lblDesc->setVisible(true);
        break;
    case 1:
        ui.lblDesc->setVisible(false);
        break;
    }
    ui.edtName->setFocus();
}

void MissModifyShelf::s_queryData()
{
    QString strName = ui.edtName->text().trimmed();
    if (strName.isEmpty())
    {
        return;
    }
    QSqlQuery sql;
    switch(ui.cBoxName->currentIndex())
    {
    case 0:
        sql.prepare(
            "SELECT GROUP_CONCAT(distinct cs_Shelf) FROM CurrentStock AS cs "
            "INNER JOIN ProductDetailInfo AS pdi ON pdi.pdi_Serial = cs.pdi_Serial "
            "INNER JOIN ProductInfo AS pi ON pi.pi_ID = pdi.pi_ID "
            "WHERE pi.pi_Name = :pi_Name;");
        sql.bindValue(":pi_Name", strName);
        break;
    case 1:
        sql.prepare("SELECT cs_Shelf FROM CurrentStock WHERE pdi_Serial = :pdi_Serial;");
        sql.bindValue(":pdi_Serial", strName);
        break;
    }

    if (sql.exec())
    {
        if (sql.next()&&!sql.value(0).isNull())
        {
            ui.edtCurrentShelf->setText(sql.value(0).toString());
        }
        else
        {
            QMessageBox::information(this,tr("抱歉"),tr("没有找到该产品的货存记录."));
            ui.edtName->setText("");
        }
    }
    else
    {
        Q_ASSERT(false);
    }
}

void MissModifyShelf::accept()
{
    QString strReason;
    if (checkForm(strReason))
    {
        QString strName = ui.edtName->text().trimmed();
        QString strShelf = ui.edtModifyShelf->text().trimmed();
        QSqlQuery sql;
        switch(ui.cBoxName->currentIndex())
        {
        case 0:
            sql.prepare(
                "UPDATE CurrentStock SET cs_Shelf = :cs_Shelf WHERE pdi_Serial IN "
                "(SELECT cs.pdi_Serial FROM CurrentStock AS cs "
                "INNER JOIN ProductDetailInfo AS pdi ON pdi.pdi_Serial = cs.pdi_Serial "
                "INNER JOIN ProductInfo AS pi ON pi.pi_ID = pdi.pi_ID "
                "WHERE pi.pi_Name = :pi_Name);");
            sql.bindValue(":cs_Shelf", strShelf);
            sql.bindValue(":pi_Name", strName);
            break;
        case 1:
            sql.prepare("UPDATE CurrentStock SET cs_Shelf = :cs_Shelf WHERE pdi_Serial = :pdi_Serial;");
            sql.bindValue(":cs_Shelf", strShelf);
            sql.bindValue(":pdi_Serial", strName);
            break;
        }
        if (sql.exec())
        {
            QDialog::accept();
        }
        else
        {
            Q_ASSERT(false);
        }
        
    }
    else
    {
        QMessageBox::information(this,tr("表单数据问题"), strReason);
    }
}

bool MissModifyShelf::checkForm( QString& strReason )
{
    bool bRet(true);
    do 
    {
        if (ui.edtName->text().trimmed().isEmpty())
        {
            strReason += tr("产品信息不能为空.");
            bRet = false;
            break;
        }

        if (ui.edtModifyShelf->text().trimmed().isEmpty())
        {
            strReason += tr("货架号不能为空.");
            bRet = false;
            break;
        }

    } while (0);
    return bRet;
}
