#include "stdafx.h"
#include "missstock.h"
#include <QtGui/QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "MissGlobal.h"
#include "misslogin.h"

bool checkDB(QSqlDatabase& db)
{
    QStringList lisTable = db.tables();
    return lisTable.contains("ProductInfo");
}

bool createConnection();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
    QFont font("Times",10);
    a.setFont(font);
    if (!createConnection())
    {
        return -1;
    }
    MissLogin Login;
    if (Login.exec() != QDialog::Accepted)
    {
        return 0;
    }

    MissStock w;
    w.show();

    
    return a.exec();
}


bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("MissDB.db3");
    if (!db.open())
    {
        qDebug()<<"Database Error";
        return false;
    }

    if (!checkDB(db))
    {
        qDebug()<<"start create tables";
        ///创建数据库
        db.transaction();
        QSqlQuery query;
        for (int ix = 0; !MissGlobal::strCreateTables[ix].isEmpty() ; ++ix)
        {
            if(!query.exec(MissGlobal::strCreateTables[ix]))
            {
                db.rollback();
                qDebug()<<"Create Tables Error.";
                return false;
            }
        }
        const QString strInsertDatas[] = 
        {
            QObject::tr("INSERT INTO OutBookEnum(ob_Reason, obe_Name) VALUES(0, '销售');"),
            QObject::tr("INSERT INTO OutBookEnum(ob_Reason, obe_Name) VALUES(1, '外借');"),
            QObject::tr("INSERT INTO PutBookEnum(pb_Reason, pbe_Name) VALUES(0, '生产');"),
            QObject::tr("INSERT INTO PutBookEnum(pb_Reason, pbe_Name) VALUES(1, '归还');"),

            QObject::tr("INSERT INTO LoanDetailEnum(ld_State, lde_Name) VALUES(0, '外借中');"),
            QObject::tr("INSERT INTO LoanDetailEnum(ld_State, lde_Name) VALUES(1, '已归还');"),
            QObject::tr("INSERT INTO LoanDetailEnum(ld_State, lde_Name) VALUES(2, '已销售');"),

            QObject::tr("INSERT INTO UserInfo(ui_User, ui_Password, ui_Right) VALUES('mj','f232e1d4dc622ef945c0a7a1b37e5d56','1');"),
            QObject::tr("INSERT INTO UserInfo(ui_User, ui_Password, ui_Right) VALUES('user','e10adc3949ba59abbe56e057f20f883e','0');"),
            ""
        };
        for (int ix = 0; !strInsertDatas[ix].isEmpty(); ++ix)
        {
            if(!query.exec(strInsertDatas[ix]))
            {
                db.rollback();
                qDebug()<<"Insert Dates Error.";
                return false;
            }
        }
        db.commit();
    }

    ///兼容性
    QStringList lisTable = db.tables();
    if (!lisTable.contains("UserInfo"))
    {
        db.transaction();
        const QString strInsertDatas[] = 
        {
            "CREATE TABLE UserInfo ("
            "ui_ID       INTEGER  PRIMARY KEY,"
            "ui_User     TEXT,"
            "ui_Password TEXT,"
            "ui_Right    TEXT"
            ");",

            "CREATE TABLE LoginLog ("
            "ll_ID       INTEGER  PRIMARY KEY,"
            "ui_ID       TEXT,"
            "ll_Date     TEXT"
            ");",

            QObject::tr("INSERT INTO UserInfo(ui_User, ui_Password, ui_Right) VALUES('mj','f232e1d4dc622ef945c0a7a1b37e5d56','1');"),
            QObject::tr("INSERT INTO UserInfo(ui_User, ui_Password, ui_Right) VALUES('user','e10adc3949ba59abbe56e057f20f883e','0');"),

            ""
        };
        QSqlQuery query;
        for (int ix = 0; !strInsertDatas[ix].isEmpty(); ++ix)
        {
            if(!query.exec(strInsertDatas[ix]))
            {
                db.rollback();
                qDebug()<<"Insert Dates Error.";
                return false;
            }
        }
        db.commit();
    }
    return true;
}
