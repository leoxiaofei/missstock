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
    ///��һ����鵱ǰ��Ʒ�Ƿ��ڵ�ǰ���� CurrentStock ��

    ///�ڶ����ڳ������� OutBook ��¼���¼

    ///�������������� �� �����ϸ�� LoanDetail ¼���¼
    ///------>>��� ������ �Ƿ���ְԱ��Ϣ�� EmployeeInfo�У�������ھ�¼��
    ///------>>��� �����û� �Ƿ��������û���Ϣ�� FinalUserInfo�У�������ڣ���¼��

    ///���Ĳ��ӵ�ǰ���� CurrentStock ��ɾ���˲�Ʒ
    QSqlDatabase::database().transaction();
    do 
    {
        QSqlQuery sql;
        ///��һ����鵱ǰ��Ʒ�Ƿ��ڵ�ǰ���� CurrentStock ��
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

        ///¼�� �������� OutBook
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
            ///�� �����ϸ�� LoanDetail ¼���¼
            sql.prepare("UPDATE LoanDetail SET ld_State = 2 WHERE pdi_Serial = :pdi_Serial AND ld_State = 0");
            sql.bindValue(":pdi_Serial", ui.edtSerial->text());
            if(!sql.exec())
            {
                break;
            }
        }
        else if(ui.combReason->currentIndex() == MissGlobal::OR_Loan)
        {
            ///��ѯְԱ��Ϣ
            sql.prepare("SELECT ei_ID FROM EmployeeInfo WHERE ei_Name = :ei_Name");
            sql.bindValue(":ei_Name", ui.combEiName->currentText());

            if(!sql.exec())
            {
                break;
            }

            if(!sql.next())
            {
                ///û���ҵ���¼, ��ô¼��ְԱ��Ϣ
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

            ///��ѯ�����û���Ϣ
            sql.prepare("SELECT fi_ID FROM FinalUserInfo WHERE fi_Name = :fi_Name");
            sql.bindValue(":fi_Name", ui.combFiName->currentText());

            if(!sql.exec())
            {
                break;
            }

            if(!sql.next())
            {
                ///û���ҵ���¼, ��ô¼�������û�
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

            ///�� �����ϸ�� LoanDetail ¼���¼
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

        ///�ӵ�ǰ������Ƴ�
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
            strReason = tr("��Ʒ���кŲ���Ϊ��.");
            break;
        }
        QSqlQuery sql;
        if (ui.combReason->currentIndex() == MissGlobal::OR_Loan)
        {
            if (ui.combEiName->currentText().trimmed().isEmpty())
            {
                strReason = tr("����˲���Ϊ��.");
                break;
            }

            if (ui.combFiName->currentText().trimmed().isEmpty())
            {
                strReason = tr("�����û�����Ϊ��.");
                break;
            }

            if (ui.dedtPlanDate->dateTime() < ui.dtePutoutDate->dateTime())
            {
                strReason = tr("�黹ʱ�䲻�����ڽ��ʱ��.");
                break;
            }

            if (ui.combPdiType->currentIndex() != MissGlobal::PT_Demo)
            {
                strReason = tr("ֻ����Ʒ���ܽ��.");
                break;
            }

            sql.prepare("SELECT * FROM CurrentStock WHERE pdi_Serial = :pdi_Serial");
            sql.bindValue(":pdi_Serial", ui.edtSerial->text());
            if(sql.exec())
            {
                if(!sql.next())
                {
                    strReason = tr("�ò�Ʒ���ڿ��У���������������\n�������Ʒ���ڿ���У��ǿ������ϴι黹ʱ��û��������������");
                    break;
                }
            }
            else
            {
                strReason = tr("���ݿ��ѯʧ�ܣ��������Bug����С�ɰɡ�");
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
                            strReason = tr("û�������ݿ��з��ָò�Ʒ����ȷ���Ƿ�������������");
                            break;
                        }
                    }
                    else
                    {
                        strReason = tr("���ݿ��ѯʧ�ܣ��������Bug����С�ɰɡ�");
                        break;
                    }
                }
            }
            else
            {
                strReason = tr("���ݿ��ѯʧ�ܣ��������Bug����С�ɰɡ�");
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
        QMessageBox::information(this, tr("����������"), strReason);
        return;
    }

    if (!saveData())
    {
        QMessageBox::information(this, tr("��Ǹ"), tr("����ʧ��."));
        return;
    }

    accept();
}

void MissPutoutDlg::s_saveAndConti()
{
    QString strReason;
    if (!checkData(strReason))
    {
        QMessageBox::information(this, tr("����������"), strReason);
        return;
    }
    if(!saveData())
    {
        QMessageBox::information(this, tr("��Ǹ"), tr("����ʧ��."));
        return;
    }

    ui.edtSerial->clear();
    showSuccess(tr("����ɹ���"));
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
    if (QMessageBox::question(this,tr("ȷ��"),tr("��ȷ�Ͻ�����Ʒת������Ʒ��"),
        QMessageBox::Yes|QMessageBox::No,QMessageBox::No) == QMessageBox::Yes)
    {
        if(changeOfficialData())
        {
            ui.combPdiType->setCurrentIndex(static_cast<int>(MissGlobal::PT_Demo));
            ui.btnOfficialChange->setEnabled(ui.combPdiType->currentIndex() == MissGlobal::PT_Official);
        }
        else
        {
            QMessageBox::information(this, tr("��Ǹ"), tr("��Ʒת��Ʒʧ��."));
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
        ///������ڵ�ǰ����У���ô��ѯ�����Ϣ��
        else
        {
            sql.prepare("SELECT pdi_Serial FROM LoanDetail WHERE pdi_Serial = :pdi_Serial AND ld_State = 0");
            sql.bindValue(":pdi_Serial", strSerial);
            if(sql.exec())
            {
                if (sql.next())
                {
                    ui.edtShelf->setText(tr("�����..."));
                }
            }
        }
    }

    ///��Ʒ��ϸ��Ϣ�� ProductDetailInfo
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
