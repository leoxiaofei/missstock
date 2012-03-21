#include "StdAfx.h"
#include "missputoutdlg.h"
#include "MissGlobal.h"
#include <QTimer>

#include <QSqlQuery>
#include "msautouppervalidator.h"

MissPutoutDlg::MissPutoutDlg(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    ui.widgetLoan->hide();
    ui.dtePutoutDate->setDateTime(QDateTime::currentDateTime());
    ui.dedtPlanDate->setDate(QDate::currentDate().addDays(30));
    queryEmployeeName();
    queryFinalUserName();
    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(s_timeup()));
    m_pTimer->setSingleShot(false);
    ui.edtSerial->setFocus();
    ui.edtSerial->setValidator(new MsAutoUpperValidator(ui.edtSerial));
}

MissPutoutDlg::~MissPutoutDlg()
{

}

bool MissPutoutDlg::saveData()
{
    ///第一步检查当前产品是否在当前库存表 CurrentStock 里

    ///第二步在出库管理表 OutBook 中录入记录

    ///第三步如果是外借 在 外借明细表 LoanDetail 录入记录
    ///------>>检查 借用人 是否在职员信息表 EmployeeInfo中，如果不在就录入
    ///------>>检查 最终用户 是否在最终用户信息表 FinalUserInfo中，如果不在，就录入

    ///第四步从当前库存表 CurrentStock 中删除此产品
    QSqlDatabase::database().transaction();
    do 
    {
        QSqlQuery sql;
        ///第一步检查当前产品是否在当前库存表 CurrentStock 里
//         sql.prepare("SELECT * FROM CurrentStock WHERE pdi_Serial = :pdi_Serial");
//         sql.bindValue(":pdi_Serial", ui.edtSerial->text());
//         if(!sql.exec())
//         {
//             break;
//         }
// 
//         if(!sql.next())
//         {
//             qDebug()<<"error: CurrentStock can't find pdi_Serial.";
//             break;
//         }

        ///录入 出库管理表 OutBook
        sql.prepare("INSERT INTO OutBook(pdi_Serial, ob_PutDate, ob_Reason) "
            "VALUES(:pdi_Serial, :ob_PutDate, :ob_Reason)");
        sql.bindValue(":pdi_Serial", ui.edtSerial->text());
        sql.bindValue(":ob_PutDate", ui.cboxPutoutNow->isChecked()?
            QDateTime::currentDateTime():ui.dtePutoutDate->dateTime());
        sql.bindValue(":ob_Reason", ui.combReason->currentIndex());
        if(!sql.exec())
        {
            break;
        }

        if(ui.combReason->currentIndex() == MissGlobal::OR_Sell)
        {
            ///在 外借明细表 LoanDetail 录入记录
            sql.prepare("UPDATE LoanDetail SET ld_State = 2 WHERE pdi_Serial = :pdi_Serial AND ld_State = 0");
            sql.bindValue(":pdi_Serial", ui.edtSerial->text());
            if(!sql.exec())
            {
                break;
            }
        }
        else if(ui.combReason->currentIndex() == MissGlobal::OR_Loan)
        {
            ///查询职员信息
            sql.prepare("SELECT ei_ID FROM EmployeeInfo WHERE ei_Name = :ei_Name");
            sql.bindValue(":ei_Name", ui.combEiName->currentText());

            if(!sql.exec())
            {
                break;
            }

            if(!sql.next())
            {
                ///没有找到记录, 那么录入职员信息
                sql.prepare("INSERT INTO EmployeeInfo(ei_Name) VALUES(:ei_Name)");
                sql.bindValue(":pi_Name", ui.combEiName->currentText());
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
            int strEi_ID = sql.value(0).toInt();

            ///查询最终用户信息
            sql.prepare("SELECT fi_ID FROM FinalUserInfo WHERE fi_Name = :fi_Name");
            sql.bindValue(":fi_Name", ui.combFiName->currentText());

            if(!sql.exec())
            {
                break;
            }

            if(!sql.next())
            {
                ///没有找到记录, 那么录入最终用户
                sql.prepare("INSERT INTO FinalUserInfo(fi_Name) VALUES(:fi_Name)");
                sql.bindValue(":fi_Name", ui.combFiName->currentText());
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
            int strFi_ID = sql.value(0).toInt();

            ///在 外借明细表 LoanDetail 录入记录
            sql.prepare("INSERT INTO LoanDetail(pdi_Serial, ld_LoanDate, ei_ID, fi_ID, ld_PlanDate, ld_State)"
                "VALUES(:pdi_Serial, :ld_LoanDate, :ei_ID, :fi_ID, :ld_PlanDate, :ld_State)");
            sql.bindValue(":pdi_Serial", ui.edtSerial->text());
            sql.bindValue(":ld_LoanDate", ui.cboxPutoutNow->isChecked()?
                QDateTime::currentDateTime():ui.dtePutoutDate->dateTime());
            sql.bindValue(":ei_ID", strEi_ID);
            sql.bindValue(":fi_ID", strFi_ID);
            sql.bindValue(":ld_PlanDate", ui.dedtPlanDate->date());
            sql.bindValue(":ld_State", static_cast<int>(MissGlobal::LS_Loan));
            if(!sql.exec())
            {
                break;
            }
        }

        ///从当前库存中移除
        sql.prepare("DELETE FROM CurrentStock WHERE pdi_Serial = :pdi_Serial");
        sql.bindValue(":pdi_Serial", ui.edtSerial->text());
        if(!sql.exec())
        {
            break;
        }

        QSqlDatabase::database().commit();
        return true;

    } while (0);

    qDebug()<<"saveData Error.";
    QSqlDatabase::database().rollback();

    return false;
}

bool MissPutoutDlg::checkData( QString& strReason )
{
    do 
    {
        if (ui.edtSerial->text().trimmed().isEmpty())
        {
            strReason = tr("产品序列号不能为空.");
            break;
        }
        QSqlQuery sql;
        if (ui.combReason->currentIndex() == MissGlobal::OR_Loan)
        {
            if (ui.combEiName->currentText().trimmed().isEmpty())
            {
                strReason = tr("借出人不能为空.");
                break;
            }

            if (ui.combFiName->currentText().trimmed().isEmpty())
            {
                strReason = tr("最终用户不能为空.");
                break;
            }

            if (ui.dedtPlanDate->dateTime() < ui.dtePutoutDate->dateTime())
            {
                strReason = tr("归还时间不能早于借出时间.");
                break;
            }

            if (ui.combPdiType->currentIndex() != MissGlobal::PT_Demo)
            {
                strReason = tr("只有样品才能借出.");
                break;
            }

            sql.prepare("SELECT * FROM CurrentStock WHERE pdi_Serial = :pdi_Serial");
            sql.bindValue(":pdi_Serial", ui.edtSerial->text());
            if(sql.exec())
            {
                if(!sql.next())
                {
                    strReason = tr("该产品不在库中，不能做外借操作。\n（如果产品就在库存中，那可能是上次归还时，没有做入库操作。）");
                    break;
                }
            }
            else
            {
                strReason = tr("数据库查询失败，这是软件Bug，找小飞吧。");
                break;
            }
        }
        else if(ui.combReason->currentIndex() == MissGlobal::OR_Sell)
        {
            sql.prepare("SELECT * FROM CurrentStock WHERE pdi_Serial = :pdi_Serial");
            sql.bindValue(":pdi_Serial", ui.edtSerial->text());
            if(sql.exec())
            {
                if(!sql.next())
                {
                    sql.prepare("SELECT pdi_Serial FROM LoanDetail WHERE pdi_Serial = :pdi_Serial AND ld_State = 0");
                    sql.bindValue(":pdi_Serial", ui.edtSerial->text());
                    if(sql.exec())
                    {
                        if(!sql.next())
                        {
                            strReason = tr("没有在数据库中发现该产品，请确认是否做过入库操作。");
                            break;
                        }
                    }
                    else
                    {
                        strReason = tr("数据库查询失败，这是软件Bug，找小飞吧。");
                        break;
                    }
                }
            }
            else
            {
                strReason = tr("数据库查询失败，这是软件Bug，找小飞吧。");
                break;
            }
        }

        return true;

    } while (0);

    return false;
}

void MissPutoutDlg::s_outReasonChanged( int nIndex)
{
    ///ObReason{OR_Sell, OR_Loan};
    switch(nIndex)
    {
    case MissGlobal::OR_Sell:
        ui.widgetLoan->hide();
        //ui.btnOfficialChange->setEnabled(false);
        break;
    case MissGlobal::OR_Loan:
        {
            ui.widgetLoan->show();
            //ui.btnOfficialChange->setEnabled(ui.combPdiType->currentIndex() == MissGlobal::PT_Official);
        }
        break;
    }
    
}

void MissPutoutDlg::s_saveAndClose()
{
    QString strReason;
    if (!checkData(strReason))
    {
        QMessageBox::information(this, tr("表单数据问题"), strReason);
        return;
    }

    if (!saveData())
    {
        QMessageBox::information(this, tr("抱歉"), tr("保存失败."));
        return;
    }

    accept();
}

void MissPutoutDlg::s_saveAndConti()
{
    QString strReason;
    if (!checkData(strReason))
    {
        QMessageBox::information(this, tr("表单数据问题"), strReason);
        return;
    }
    if(!saveData())
    {
        QMessageBox::information(this, tr("抱歉"), tr("保存失败."));
        return;
    }

    ui.edtSerial->clear();
    showSuccess(tr("保存成功！"));
}

void MissPutoutDlg::s_checkSerial()
{
    QString strSerial = ui.edtSerial->text().trimmed();
    if (strSerial.isEmpty())
    {
        return;
    }
    querySerial(strSerial);
}

void MissPutoutDlg::s_officialChange()
{
    if (QMessageBox::question(this,tr("确认"),tr("请确认将此正品转换成样品吗？"),
        QMessageBox::Yes|QMessageBox::No,QMessageBox::No) == QMessageBox::Yes)
    {
        if(changeOfficialData())
        {
            ui.combPdiType->setCurrentIndex(static_cast<int>(MissGlobal::PT_Demo));
            ui.btnOfficialChange->setEnabled(ui.combPdiType->currentIndex() == MissGlobal::PT_Official);
        }
        else
        {
            QMessageBox::information(this, tr("抱歉"), tr("正品转样品失败."));
        }
    }
}

bool MissPutoutDlg::changeOfficialData()
{
    QSqlDatabase::database().transaction();
    do
    {
        QSqlQuery sql;
        sql.prepare("UPDATE ProductDetailInfo SET pdi_Type = :pdi_Type WHERE pdi_Serial = :pdi_Serial");
        sql.bindValue(":pdi_Type", static_cast<int>(MissGlobal::PT_Demo));
        sql.bindValue(":pdi_Serial", ui.edtSerial->text());
        if(!sql.exec())
        {
            break;
        }
        QSqlDatabase::database().commit();
        return true;
    }
    while(0);

    QSqlDatabase::database().rollback();
    return false;
}

bool MissPutoutDlg::querySerial( const QString& strSerial )
{
    QSqlQuery sql;
    sql.prepare("SELECT cs_Shelf FROM CurrentStock WHERE pdi_Serial = :pdi_Serial");
    sql.bindValue(":pdi_Serial", strSerial);
    if(sql.exec())
    {
        if(sql.next())
        {
            ui.edtShelf->setText(sql.value(0).toString());
        }
        ///如果不在当前库存中，那么查询外借信息表
        else
        {
            sql.prepare("SELECT pdi_Serial FROM LoanDetail WHERE pdi_Serial = :pdi_Serial AND ld_State = 0");
            sql.bindValue(":pdi_Serial", strSerial);
            if(sql.exec())
            {
                if (sql.next())
                {
                    ui.edtShelf->setText(tr("外借中..."));
                }
            }
        }
    }

    ///产品详细信息表 ProductDetailInfo
    sql.prepare("SELECT pi_ID, pdi_Type, pdi_Desc FROM ProductDetailInfo WHERE pdi_Serial = :pdi_Serial");
    sql.bindValue(":pdi_Serial", strSerial);
    if(sql.exec())
    {
        if(sql.next())
        {
            int nID = sql.value(0).toInt();
            ui.combPdiType->setCurrentIndex(sql.value(1).toInt());
            ui.btnOfficialChange->setEnabled(ui.combPdiType->currentIndex() == MissGlobal::PT_Official);
            ui.pedtPdiDesc->setPlainText(sql.value(2).toString());
            sql.prepare("SELECT * FROM ProductInfo WHERE pi_ID = :pi_ID");
            sql.bindValue(":pi_ID", nID);
            if(sql.exec())
            {
                if(sql.next())
                {
                    ui.edtProdName->setText(sql.value(1).toString());
                    ui.pedtPiDesc->setPlainText(sql.value(2).toString());
                }
            }


        }
    }
    return true;
}

void MissPutoutDlg::queryEmployeeName()
{
    ui.combEiName->addItem("");
    QSqlQuery sql;
    if(sql.exec("SELECT ei_Name FROM EmployeeInfo"))
    {
        while(sql.next())
        {
            ui.combEiName->addItem(sql.value(0).toString());
        }
    }
}

void MissPutoutDlg::queryFinalUserName()
{
    QSqlQuery sql;
    ui.combFiName->addItem("");
    if(sql.exec("SELECT fi_Name FROM FinalUserInfo"))
    {
        while(sql.next())
        {
            ui.combFiName->addItem(sql.value(0).toString());
        }
    }
}

void MissPutoutDlg::showSuccess( const QString& strTips )
{
    ui.lblTips->setText(strTips);
    ui.lblTips->show();
    ui.btnSaveAndConti->setEnabled(false);
    m_pTimer->start(3000);
}

void MissPutoutDlg::s_timeup()
{
    ui.lblTips->hide();
    ui.btnSaveAndConti->setEnabled(true);
}
