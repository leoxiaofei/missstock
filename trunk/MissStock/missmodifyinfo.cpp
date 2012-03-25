#include "StdAfx.h"
#include "missmodifyinfo.h"

#include <QSqlQuery>
#include <QMessageBox>
#include "msautouppervalidator.h"

MissModifyInfo::MissModifyInfo(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    ui.edtError->setValidator(new MsAutoUpperValidator(ui.edtError));
    ui.edtName->setValidator(new MsAutoUpperValidator(ui.edtName));
    s_changeName(0);
}

MissModifyInfo::~MissModifyInfo()
{

}

void MissModifyInfo::s_changeName( int nIndex )
{
    ui.lblName->setText(ui.cBoxName->itemText(nIndex%2));
    switch (nIndex)
    {
    case 0:
    case 1:
        ui.edtError->setEnabled(true);
        ui.lblDesc->setVisible(false);
        ui.pedtDesc->setVisible(false);
        break;
    case 2:
    case 3:
        ui.edtError->setText("");
        ui.edtError->setEnabled(false);
        ui.lblDesc->setText(ui.cBoxName->itemText(nIndex));
        ui.lblDesc->setVisible(true);
        ui.pedtDesc->setVisible(true);
        ui.edtName->setFocus();
        break;
    }
}

void MissModifyInfo::s_checkNameE()
{
    QString strName = ui.edtError->text().trimmed();
    if (strName.isEmpty())
    {
        return;
    }
    QSqlQuery sql;
    switch(ui.cBoxName->currentIndex())
    {
    case 0:     ///产品型号
        sql.prepare("SELECT pi_Name FROM ProductInfo WHERE pi_Name = :pi_Name");
        sql.bindValue(":pi_Name", strName);

        if(sql.exec())
        {
            if(sql.next())
            {
                ui.edtName->setText(sql.value(0).toString());
            }
            else
            {
                qDebug()<<"Can't find pi_Name";
                QMessageBox::information(this, tr("抱歉"), tr("没有找到此产品型号."));
                ui.edtError->setText("");
            }
        }
        else
        {
            Q_ASSERT(false);
        }
        
        break;
    case 1:     ///序列号
        sql.prepare("SELECT pdi_Serial FROM ProductDetailInfo WHERE pdi_Serial = :pdi_Serial");
        sql.bindValue(":pdi_Serial", strName);

        if(sql.exec())
        {
            if(sql.next())
            {
                ui.edtName->setText(sql.value(0).toString());
            }
            else
            {
                qDebug()<<"Can't find pi_Name";
                QMessageBox::information(this, tr("抱歉"), tr("没有找到此序列号."));
                ui.edtError->setText("");
            }
        }
        else
        {
            Q_ASSERT(false);
        }
        break;
    }
}

void MissModifyInfo::s_checkNameR()
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
        break;
    case 1:
        break;
    case 2:
        sql.prepare("SELECT pi_Desc FROM ProductInfo WHERE pi_Name = :pi_Name");
        sql.bindValue(":pi_Name", strName);

        if(sql.exec())
        {
            if(sql.next())
            {
                ui.pedtDesc->setPlainText(sql.value(0).toString());
            }
            else
            {
                qDebug()<<"Can't find pi_Name";
                QMessageBox::information(this, tr("抱歉"), tr("没有找到此产品型号."));
                ui.edtName->setText("");
            }
        }
        else
        {
            Q_ASSERT(false);
        }
        break;
    case 3:
        sql.prepare("SELECT pdi_Desc FROM ProductDetailInfo WHERE pdi_Serial = :pdi_Serial");
        sql.bindValue(":pdi_Serial", strName);

        if(sql.exec())
        {
            if(sql.next())
            {
                ui.pedtDesc->setPlainText(sql.value(0).toString());
            }
            else
            {
                qDebug()<<"Can't find pi_Name";
                QMessageBox::information(this, tr("抱歉"), tr("没有找到此序列号."));
                ui.edtName->setText("");
            }
        }
        else
        {
            Q_ASSERT(false);
        }
        break;
    }
}

void MissModifyInfo::accept()
{
    QString strReason;
    if (checkForm(strReason))
    {
        QSqlQuery sql;
        switch(ui.cBoxName->currentIndex())
        {
        case 0:
            sql.prepare("UPDATE ProductInfo SET pi_Name = :pi_Name WHERE pi_Name = :pi_Name_Old");
            sql.bindValue(":pi_Name",ui.edtName->text().trimmed());
            sql.bindValue(":pi_Name_Old",ui.edtError->text().trimmed());
            if (!sql.exec())
            {
                Q_ASSERT(false);
            }
            break;
        case 1:
            sql.prepare("UPDATE CurrentStock SET pdi_Serial = :pdi_Serial WHERE pdi_Serial = :pdi_Serial_Old");
            sql.bindValue(":pdi_Serial",ui.edtName->text().trimmed());
            sql.bindValue(":pdi_Serial_Old",ui.edtError->text().trimmed());
            if (!sql.exec())
            {
                Q_ASSERT(false);
            }
            sql.prepare("UPDATE LoanDetail SET pdi_Serial = :pdi_Serial WHERE pdi_Serial = :pdi_Serial_Old");
            sql.bindValue(":pdi_Serial",ui.edtName->text().trimmed());
            sql.bindValue(":pdi_Serial_Old",ui.edtError->text().trimmed());
            if (!sql.exec())
            {
                Q_ASSERT(false);
            }
            sql.prepare("UPDATE OutBook SET pdi_Serial = :pdi_Serial WHERE pdi_Serial = :pdi_Serial_Old");
            sql.bindValue(":pdi_Serial",ui.edtName->text().trimmed());
            sql.bindValue(":pdi_Serial_Old",ui.edtError->text().trimmed());
            if (!sql.exec())
            {
                Q_ASSERT(false);
            }
            sql.prepare("UPDATE ProductDetailInfo SET pdi_Serial = :pdi_Serial WHERE pdi_Serial = :pdi_Serial_Old");
            sql.bindValue(":pdi_Serial",ui.edtName->text().trimmed());
            sql.bindValue(":pdi_Serial_Old",ui.edtError->text().trimmed());
            if (!sql.exec())
            {
                Q_ASSERT(false);
            }
            sql.prepare("UPDATE PutBook SET pdi_Serial = :pdi_Serial WHERE pdi_Serial = :pdi_Serial_Old");
            sql.bindValue(":pdi_Serial",ui.edtName->text().trimmed());
            sql.bindValue(":pdi_Serial_Old",ui.edtError->text().trimmed());
            if (!sql.exec())
            {
                Q_ASSERT(false);
            }
            break;
        case 2:
            sql.prepare("UPDATE ProductInfo SET pi_Desc = :pi_Desc WHERE pi_Name = :pi_Name");
            sql.bindValue(":pi_Desc",ui.pedtDesc->toPlainText().trimmed());
            sql.bindValue(":pi_Name",ui.edtName->text().trimmed());
            if (!sql.exec())
            {
                Q_ASSERT(false);
            }
            break;
        case 3:
            sql.prepare("UPDATE ProductDetailInfo SET pdi_Desc = :pdi_Desc WHERE pdi_Serial = :pdi_Serial");
            sql.bindValue(":pdi_Desc",ui.pedtDesc->toPlainText().trimmed());
            sql.bindValue(":pdi_Serial",ui.edtName->text().trimmed());
            if (!sql.exec())
            {
                Q_ASSERT(false);
            }
            break;
        }
        QDialog::accept();
    }
    else
    {
        QMessageBox::information(this,tr("抱歉"),strReason);
    }

    
}

bool MissModifyInfo::checkForm( QString& strReason )
{
    bool bRet(false);
    QString strError = ui.edtError->text().trimmed();
    QString strName = ui.edtName->text().trimmed();
    switch(ui.cBoxName->currentIndex())
    {
    case 0:
        if (strError.isEmpty())
        {
            strReason += tr("错误的产品名不能为空.");
            break;
        }
        if (strName.isEmpty())
        {
            strReason += tr("正确的产品名不能为空.");
            break;
        }
        ///检查即将修改的名称是否在库里存在
        {
            QSqlQuery sql;
            sql.prepare("SELECT pi_ID FROM ProductInfo WHERE pi_Name = :pi_Name;");
            sql.bindValue(":pi_Name",strName);
            if (sql.exec())
            {
                if (sql.next())
                {
                    strReason += tr("此产品名已存在.");
                    break;
                }
                bRet = true;
            }
            else
            {
                Q_ASSERT(false);
            }
        }
        break;
    case 1:
        if (strError.isEmpty())
        {
            strReason += tr("错误的序列号不能为空.");
            break;
        }
        if (strName.isEmpty())
        {
            strReason += tr("正确的序列号不能为空.");
            break;
        }
        ///检查即将修改的名称是否在库里存在
        {
            QSqlQuery sql;
            sql.prepare("SELECT pdi_Serial FROM ProductDetailInfo WHERE pdi_Serial = :pdi_Serial;");
            sql.bindValue(":pdi_Serial",strName);
            if (sql.exec())
            {
                if (sql.next())
                {
                    strReason += tr("此序列号已存在.");
                    break;
                }
                bRet = true;
            }
            else
            {
                Q_ASSERT(false);
            }
        }
        break;
    case 2:
        if (strName.isEmpty())
        {
            strReason += tr("产品名不能为空.");
            break;
        }
        bRet = true;
        break;
    case 3:
        if (strName.isEmpty())
        {
            strReason += tr("序列号不能为空.");
            break;
        }
        bRet = true;
        break;
    }

    return bRet;
}
