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
    ///��һ����ѯ����Ʒ��ϸ��Ϣ�� ProductDetailInfo�����Ƿ�Ǽǹ�������Ϣ
    ///���û�еǼǹ�����ô¼���Ʒ��Ϣ�Ͳ�Ʒ��ϸ��Ϣ��
    ///�ڶ�����¼�� ��ǰ���� CurrentStock
    ///��������¼�� ������� PutBook

    QSqlDatabase::database().transaction();

    do 
    {
        QSqlQuery sql;
        ///��һ����ѯ����Ʒ��ϸ��Ϣ�� ProductDetailInfo�����Ƿ�Ǽǹ�������Ϣ
        sql.prepare("SELECT * FROM ProductDetailInfo WHERE pdi_Serial = :pdi_Serial");
        sql.bindValue(":pdi_Serial", ui.edtSerial->text());
        if(!sql.exec())
        {
            break;
        }
        if(!sql.next())
        {
            ///���û���ҵ���¼����ô��ѯ��Ʒ��Ϣ
            sql.prepare("SELECT pi_ID FROM ProductInfo WHERE pi_Name = :pi_Name");
            sql.bindValue(":pi_Name", ui.edtProdName->text());

            if(!sql.exec())
            {
                break;
            }
            if(!sql.next())
            {
                ///û���ҵ���¼, ��ô¼���Ʒ��Ϣ
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
            ///¼���Ʒ��ϸ��Ϣ
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

        ///¼�� ��ǰ���� CurrentStock
        sql.prepare("INSERT INTO CurrentStock(pdi_Serial, cs_Shelf) VALUES(:pdi_Serial, :cs_Shelf)");
        sql.bindValue(":pdi_Serial", ui.edtSerial->text());
        sql.bindValue(":cs_Shelf", ui.edtShelf->text());
        if(!sql.exec())
        {
            break;
        }

        ///¼�� ������� PutBook
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
            ///��ѯ�������
            ///�� �����ϸ�� LoanDetail ¼���¼
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
        QMessageBox::information(this, tr("����������"), strReason);
        return;
    }
    if(!saveData())
    {
        QMessageBox::information(this, tr("����ʧ��"), tr("����ʧ��"));
        return;
    }

    ui.edtSerial->clear();
    ui.pedtPdiDesc->clear();
    showSuccess(tr("����ɹ���"));
}

void MissPutinDlg::s_saveAndClose()
{
    QString strReason;
    if (!checkData(strReason))
    {
        QMessageBox::information(this, tr("����������"), strReason);
        return;
    }

    if (!saveData())
    {
        QMessageBox::information(this, tr("����ʧ��"), tr("����ʧ��"));
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
        ///���û���ҵ�����ô�������к�ǰ��λ����Ʒ�ͺ���
        if(!querySerial(strSerial))
        {
            ui.edtProdName->setText(strSerial.left(5).insert(1,"-"));
        }
    }
    else
    {
        QMessageBox::information(this, tr("����"), tr("�˼���Ʒ���ڿ����,��Ӧ����������."));
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
            strReason = tr("���������к�.");
            break;
        }

        if (ui.edtProdName->text().trimmed().isEmpty())
        {
            strReason = tr("�������Ʒ��.");
            break;
        }

        if (ui.edtShelf->text().trimmed().isEmpty())
        {
            strReason = tr("��������ܺ�.");
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
//                     strReason = tr("�˲�Ʒ�ڿ���У��޷�����������");
//                     break;
//                 }
//             }

            sql.prepare("SELECT pdi_Serial FROM LoanDetail WHERE pdi_Serial = :pdi_Serial AND ld_State = 0");
            sql.bindValue(":pdi_Serial", ui.edtSerial->text());
            if(sql.exec())
            {
                if(sql.next())
                {
                    strReason = tr("�ò�Ʒ�Ѿ�����ˣ���֪����ô��������������");
                    break;
                }
            }
            sql.prepare("SELECT pdi_Serial FROM OutBook WHERE pdi_Serial = :pdi_Serial AND ob_Reason = 0");
            sql.bindValue(":pdi_Serial", ui.edtSerial->text());
            if(sql.exec())
            {
                if(sql.next())
                {
                    strReason = tr("�ò�Ʒ�Ѿ������ˣ���֪����ô��������������");
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
                    strReason = tr("�ò�Ʒû�н����¼�����ѵ�����һ�ν��û�еǼǣ������Ѿ��黹�ˣ���");
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
    ///��Ʒ��ϸ��Ϣ�� ProductDetailInfo
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

            ///��ѯ���ܺŴ���
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

            ///��ѯ���ܺŴ���
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
    ///��ѯ ��ǰ���� CurrentStock �������,���޷����
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
