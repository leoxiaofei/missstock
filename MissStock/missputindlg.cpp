#include "StdAfx.h"
#include "missputindlg.h"

#include <QSqlQuery>
#include "MissGlobal.h"
#include "msautouppervalidator.h"

MissPutinDlg::MissPutinDlg(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    ui.lblTips->hide();
    ui.dtePutinDate->setDateTime(QDateTime::currentDateTime());
    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(s_timeup()));
    m_pTimer->setSingleShot(false);
    ui.edtSerial->setFocus();
    ui.edtProdName->setValidator(new MsAutoUpperValidator(ui.edtProdName));
    ui.edtSerial->setValidator(new MsAutoUpperValidator(ui.edtSerial));
    ui.edtShelf->setValidator(new MsAutoUpperValidator(ui.edtShelf));
}

MissPutinDlg::~MissPutinDlg()
{

}

bool MissPutinDlg::saveData()
{
    ///第一步查询：产品详细信息表 ProductDetailInfo，看是否登记过此条信息
    ///如果没有登记过，那么录入产品信息和产品详细信息；
    ///第二步：录入 当前库存表 CurrentStock
    ///第三步：录入 入库管理表 PutBook

    QSqlDatabase::database().transaction();

    do 
    {
        QSqlQuery sql;
        ///第一步查询：产品详细信息表 ProductDetailInfo，看是否登记过此条信息
        sql.prepare("SELECT * FROM ProductDetailInfo WHERE pdi_Serial = :pdi_Serial");
        sql.bindValue(":pdi_Serial", ui.edtSerial->text());
        if(!sql.exec())
        {
            break;
        }
        if(!sql.next())
        {
            ///如果没有找到记录，那么查询产品信息
            sql.prepare("SELECT pi_ID FROM ProductInfo WHERE pi_Name = :pi_Name");
            sql.bindValue(":pi_Name", ui.edtProdName->text());

            if(!sql.exec())
            {
                break;
            }
            if(!sql.next())
            {
                ///没有找到记录, 那么录入产品信息
                sql.prepare("INSERT INTO ProductInfo(pi_Name, pi_Desc) VALUES(:pi_Name, :pi_Desc)");
                sql.bindValue(":pi_Name", ui.edtProdName->text());
                sql.bindValue(":pi_Desc", ui.pedtPiDesc->toPlainText());
                if(!sql.exec())
                {
                    break;
                }
                if(!sql.exec("SELECT last_insert_rowid()"))
                {
                    break;
                }
                sql.next();
            }
            int strPi_ID = sql.value(0).toInt();
            ///录入产品详细信息
            sql.prepare("INSERT INTO ProductDetailInfo(pdi_Serial, pi_ID, pdi_Type, pdi_Desc) "
                "VALUES(:pdi_Serial, :pi_ID, :pdi_Type, :pdi_Desc)");
            sql.bindValue(":pdi_Serial", ui.edtSerial->text());
            sql.bindValue(":pi_ID",strPi_ID);
            sql.bindValue(":pdi_Type", ui.combPdiType->currentIndex());
            sql.bindValue(":pdi_Desc", ui.pedtPdiDesc->toPlainText());
            if(!sql.exec())
            {
                break;
            }
        }

        ///录入 当前库存表 CurrentStock
        sql.prepare("INSERT INTO CurrentStock(pdi_Serial, cs_Shelf) VALUES(:pdi_Serial, :cs_Shelf)");
        sql.bindValue(":pdi_Serial", ui.edtSerial->text());
        sql.bindValue(":cs_Shelf", ui.edtShelf->text());
        if(!sql.exec())
        {
            break;
        }

        ///录入 入库管理表 PutBook
        sql.prepare("INSERT INTO PutBook(pdi_Serial, pb_PutDate, pb_Reason) "
            "VALUES(:pdi_Serial, :pb_PutDate, :pb_Reason)");
        sql.bindValue(":pdi_Serial", ui.edtSerial->text());
        sql.bindValue(":pb_PutDate", ui.cboxPutinNow->isChecked()?
            QDateTime::currentDateTime():ui.dtePutinDate->dateTime());
        sql.bindValue(":pb_Reason", ui.combReason->currentIndex());
        if(!sql.exec())
        {
            break;
        }

        if(ui.combReason->currentIndex() == MissGlobal::PR_Return)
        {
            ///查询外借管理表
            ///在 外借明细表 LoanDetail 录入记录
            sql.prepare("UPDATE LoanDetail SET ld_State = 1 WHERE pdi_Serial = :pdi_Serial AND ld_State = 0");
            sql.bindValue(":pdi_Serial", ui.edtSerial->text());
            if(!sql.exec())
            {
                break;
            }
        }

        QSqlDatabase::database().commit();
        return true;

    } while (0);
    
    qDebug()<<"saveData Error.";
    QSqlDatabase::database().rollback();

    return false;
}

void MissPutinDlg::s_saveAndConti()
{
    QString strReason;
    if (!checkData(strReason))
    {
        QMessageBox::information(this, tr("表单数据问题"), strReason);
        return;
    }
    if(!saveData())
    {
        QMessageBox::information(this, tr("保存失败"), tr("保存失败"));
        return;
    }

    ui.edtSerial->clear();
    ui.pedtPdiDesc->clear();
    showSuccess(tr("保存成功！"));
}

void MissPutinDlg::s_saveAndClose()
{
    QString strReason;
    if (!checkData(strReason))
    {
        QMessageBox::information(this, tr("表单数据问题"), strReason);
        return;
    }

    if (!saveData())
    {
        QMessageBox::information(this, tr("保存失败"), tr("保存失败"));
        return;
    }

    accept();
}

void MissPutinDlg::s_querySerial()
{
    QString strSerial = ui.edtSerial->text().trimmed();
    if (strSerial.isEmpty())
    {
        return;
    }
    
    if (!existSerial(strSerial))
    {
        ///如果没有找到，那么输入序列号前五位到产品型号里
        if(!querySerial(strSerial))
        {
            ui.edtProdName->setText(strSerial.left(5).insert(1,"-"));
        }
    }
    else
    {
        QMessageBox::information(this, tr("疑问"), tr("此件产品已在库存中,不应再做入库操作."));
        ui.edtSerial->setText("");
    }
}

void MissPutinDlg::s_queryProdName()
{
    queryProdName(ui.edtProdName->text().trimmed());
}

bool MissPutinDlg::checkData( QString& strReason )
{
    do 
    {
        if (ui.edtSerial->text().trimmed().isEmpty())
        {
            strReason = tr("请输入序列号.");
            break;
        }

        if (ui.edtProdName->text().trimmed().isEmpty())
        {
            strReason = tr("请输入产品名.");
            break;
        }

        if (ui.edtShelf->text().trimmed().isEmpty())
        {
            strReason = tr("请输入货架号.");
            break;
        }

        QSqlQuery sql;
        if (ui.combReason->currentIndex() == MissGlobal::PR_Produce)
        {
//             sql.prepare("SELECT pdi_Serial FROM CurrentStock WHERE pdi_Serial = :pdi_Serial");
//             sql.bindValue(":pdi_Serial", ui.edtSerial->text());
//             if(sql.exec())
//             {
//                 if(sql.next())
//                 {
//                     strReason = tr("此产品在库存中，无法做入库操作。");
//                     break;
//                 }
//             }

            sql.prepare("SELECT pdi_Serial FROM LoanDetail WHERE pdi_Serial = :pdi_Serial AND ld_State = 0");
            sql.bindValue(":pdi_Serial", ui.edtSerial->text());
            if(sql.exec())
            {
                if(sql.next())
                {
                    strReason = tr("该产品已经借出了，不知道怎么做生产入库操作。");
                    break;
                }
            }
            sql.prepare("SELECT pdi_Serial FROM OutBook WHERE pdi_Serial = :pdi_Serial AND ob_Reason = 0");
            sql.bindValue(":pdi_Serial", ui.edtSerial->text());
            if(sql.exec())
            {
                if(sql.next())
                {
                    strReason = tr("该产品已经出售了，不知道怎么做生产入库操作。");
                    break;
                }
            }
        }
        else if(ui.combReason->currentIndex() == MissGlobal::PR_Return)
        {
            sql.prepare("SELECT pdi_Serial FROM LoanDetail WHERE pdi_Serial = :pdi_Serial AND ld_State = 0");
            sql.bindValue(":pdi_Serial", ui.edtSerial->text());
            if(sql.exec())
            {
                if(!sql.next())
                {
                    strReason = tr("该产品没有借出记录。（难道是上一次借出没有登记？或者已经归还了？）");
                    break;
                }
            }
        }

        return true;

    } while (0);

    return false;
}

bool MissPutinDlg::querySerial(const QString& strSerial)
{
    bool bRet(false);
    QSqlQuery sql;
    ///产品详细信息表 ProductDetailInfo
    sql.prepare("SELECT * FROM ProductDetailInfo WHERE pdi_Serial = :pdi_Serial");
    sql.bindValue(":pdi_Serial", strSerial);
    if(sql.exec())
    {
        if(sql.next())
        {
            int nID = sql.value(1).toInt();
            ui.combPdiType->setCurrentIndex(sql.value(2).toInt());
            ui.pedtPdiDesc->setPlainText(sql.value(3).toString());
            sql.prepare("SELECT * FROM ProductInfo WHERE pi_ID = :pi_ID");
            sql.bindValue(":pi_ID", nID);
            if(sql.exec())
            {
                if(sql.next())
                {
                    ui.edtProdName->setText(sql.value(1).toString());
                    ui.pedtPiDesc->setPlainText(sql.value(2).toString());
                    bRet = true;
                }
            }

            ///查询货架号代码
            sql.prepare(
                "SELECT cs.cs_Shelf FROM ProductDetailInfo AS pdi "
                "INNER JOIN CurrentStock AS cs ON pdi.pdi_Serial = cs.pdi_Serial "
                "WHERE pdi.pi_ID = :pi_ID");
            sql.bindValue(":pi_ID", nID);
            if(sql.exec())
            {
                if(sql.next())
                {
                    ui.edtShelf->setText(sql.value(0).toString());
                }
            }
            
        }
    }
    return bRet;
}

bool MissPutinDlg::queryProdName( const QString& strProdName )
{
    bool bRet(false);
    QSqlQuery sql;
    sql.prepare("SELECT pi_ID,pi_Desc FROM ProductInfo WHERE pi_Name = :pi_Name");
    sql.bindValue(":pi_Name", strProdName);
    if(sql.exec())
    {
        if(sql.next())
        {
            ui.pedtPiDesc->setPlainText(sql.value(1).toString());

            ///查询货架号代码
            int strPIID = sql.value(0).toInt();
            sql.prepare(
                "SELECT cs.cs_Shelf FROM ProductDetailInfo AS pdi "
                "INNER JOIN CurrentStock AS cs ON pdi.pdi_Serial = cs.pdi_Serial "
                "WHERE pdi.pi_ID = :pi_ID");
            sql.bindValue(":pi_ID", strPIID);
            if(sql.exec())
            {
                if(sql.next())
                {
                    ui.edtShelf->setText(sql.value(0).toString());
                }
            }


            bRet = true;
        }
    }
    return bRet;
}

bool MissPutinDlg::existSerial( const QString& strSerial )
{
    bool bRet(false);
    ///查询 当前库存表 CurrentStock 如果存在,则无法入库
    QSqlQuery sql;
    sql.prepare("SELECT pdi_Serial FROM CurrentStock WHERE pdi_Serial = :pdi_Serial");
    sql.bindValue(":pdi_Serial", strSerial);
    if(sql.exec())
    {   
        if(sql.next())
        {
            bRet = true;
        }
    }

    return bRet;
}

void MissPutinDlg::s_timeup()
{
    ui.lblTips->hide();
    ui.btnSaveAndConti->setEnabled(true);
}

void MissPutinDlg::showSuccess( const QString& strTips )
{
    ui.lblTips->setText(strTips);
    ui.lblTips->show();
    ui.btnSaveAndConti->setEnabled(false);
    m_pTimer->start(3000);
}
