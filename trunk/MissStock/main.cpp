#include "stdafx.h"
#include "missstock.h"
#include <QtGui/QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "MissGlobal.h"

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
    return true;
}
