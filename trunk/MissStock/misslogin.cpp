#include "StdAfx.h"
#include "misslogin.h"
#include <QSqlQuery>
#include "MissGlobal.h"
#include "MissTools.h"

MissLogin::MissLogin(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
}

MissLogin::~MissLogin()
{

}

void MissLogin::accept()
{
    QString strReason;
    if (!checkForm(strReason))
    {
        QMessageBox::information(this,tr("表单数据问题"), strReason);
        return ;
    }
    QSqlQuery sql;
    QString cs = MissTools::MD5(ui.edtPassword->text().trimmed());
    sql.prepare("SELECT ui_ID,ui_Right FROM UserInfo WHERE ui_User = :ui_User AND ui_Password = :ui_Password");
    sql.bindValue(":ui_User",ui.edtUser->text().trimmed().toLower());
    sql.bindValue(":ui_Password", MissTools::MD5(ui.edtPassword->text().trimmed()));
    if (sql.exec())
    {
        if (sql.next())
        {
            MissGlobal::userInfo.nUserID = sql.value(0).toInt();
            MissGlobal::userInfo.strRight = sql.value(1).toString();
            sql.prepare("INSERT INTO LoginLog(ui_ID, ll_Date) VALUES(:ui_ID, datetime('now','localtime'))");
            sql.bindValue(":ui_ID", MissGlobal::userInfo.nUserID);
            if(!sql.exec())
            {
                Q_ASSERT(false);
            }
            QDialog::accept();
        }
        else
        {
            QMessageBox::information(this,tr("抱歉"),tr("用户名或密码错误."));
        }
    }
    
}

bool MissLogin::checkForm( QString& strReason )
{
    bool bRet(true);
    do 
    {
        if (ui.edtUser->text().trimmed().isEmpty())
        {
            strReason += tr("用户名不能为空.");
            bRet = false;
            break;
        }

        if (ui.edtPassword->text().trimmed().isEmpty())
        {
            strReason += tr("密码不能为空.");
            bRet = false;
            break;
        }

    } while (0);

    return bRet;
}
