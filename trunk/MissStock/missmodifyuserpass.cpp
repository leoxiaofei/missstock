#include "StdAfx.h"
#include "missmodifyuserpass.h"
#include <QSqlQuery>
#include "MissGlobal.h"
#include "MissTools.h"

MissModifyUserPass::MissModifyUserPass(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    queryUser();
    ui.edtOldPass->setFocus();
}

MissModifyUserPass::~MissModifyUserPass()
{

}

void MissModifyUserPass::accept()
{
    QString strReason;
    if (checkForm(strReason))
    {
        QSqlDatabase::database().transaction();
        QSqlQuery sql;
        bool bSuccess(true);
        QString strData = ui.edtNewUser->text().trimmed().toLower();
        if (!strData.isEmpty())
        {
            sql.prepare("UPDATE UserInfo SET ui_User = :ui_User WHERE ui_ID = :ui_ID");
            sql.bindValue(":ui_ID", MissGlobal::userInfo.nUserID);
            sql.bindValue(":ui_User", strData);
            if(!sql.exec())
            {
                bSuccess = false;
                Q_ASSERT(false);
            }
        }

        strData = ui.edtNewPass->text();
        if (!strData.isEmpty())
        {
            sql.prepare("UPDATE UserInfo SET ui_Password = :ui_Password WHERE ui_ID = :ui_ID");
            sql.bindValue(":ui_ID", MissGlobal::userInfo.nUserID);
            sql.bindValue(":ui_Password", MissTools::MD5(strData));
            if(!sql.exec())
            {
                bSuccess = false;
                Q_ASSERT(false);
            }
        }

        if (bSuccess)
        {
            QSqlDatabase::database().commit();
        }
        else
        {
            QSqlDatabase::database().rollback();
        }

        QDialog::accept();
    }
    else
    {
        QMessageBox::information(this,tr("表单数据错误"),strReason);
    }
    
}

void MissModifyUserPass::queryUser()
{
    QSqlQuery sql;
    sql.prepare("SELECT ui_User FROM UserInfo WHERE ui_ID = :ui_ID");
    sql.bindValue(":ui_ID", MissGlobal::userInfo.nUserID);
    if(sql.exec())
    {
        sql.next();
        ui.edtOldUser->setText(sql.value(0).toString());
    }
}

bool MissModifyUserPass::checkForm( QString& strReason )
{
    bool bRet(true);
    do 
    {
        if (ui.edtOldPass->text().isEmpty())
        {
            strReason += tr("旧密码不能为空.");
            bRet = false;
            break;
        }

        if (ui.edtNewPass->text() != ui.edtNewPassAG->text())
        {
            strReason += tr("两次输入的密码不同.");
            bRet = false;
            break;
        }

        QSqlQuery sql;
        sql.prepare("SELECT ui_ID FROM UserInfo WHERE ui_Password = :ui_Password AND ui_ID = :ui_ID");
        sql.bindValue(":ui_Password",MissTools::MD5(ui.edtOldPass->text()));
        sql.bindValue(":ui_ID",MissGlobal::userInfo.nUserID);
        if (sql.exec())
        {
            if (!sql.next())
            {
                strReason += tr("当前用户密码错误，无法修改信息.");
                bRet = false;
                break;
            }
        }
        else
        {
            Q_ASSERT(false);
        }

        QString strName = ui.edtNewUser->text().trimmed().toLower();
        if (!strName.isEmpty())
        {
            sql.prepare("SELECT ui_User FROM UserInfo WHERE ui_User = :ui_User");
            sql.bindValue(":ui_User", strName);
            if (sql.exec())
            {
                if (sql.next())
                {
                    strReason += tr("新用户名已被使用，请更换.");
                    bRet = false;
                    break;
                }
            }
            else
            {
                Q_ASSERT(false);
            }
        }
    } while (0);
    return bRet;
}
