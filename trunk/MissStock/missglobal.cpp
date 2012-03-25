#include "StdAfx.h"
#include "MissGlobal.h"

namespace MissGlobal
{
    const QString strCreateTables[] = 
    {
        "CREATE TABLE ProductInfo ("
        "pi_ID   INTEGER  PRIMARY KEY AUTOINCREMENT,"
        "pi_Name TEXT,"
        "pi_Desc TEXT"
        ");",

        "CREATE TABLE ProductDetailInfo ("
        "pdi_Serial   TEXT  PRIMARY KEY,"
        "pi_ID        INTEGER,"
        "pdi_Type     INTEGER,"
        "pdi_Desc     TEXT"
        ");",

        "CREATE TABLE LoanDetail ("
        "ld_ID       INTEGER  PRIMARY KEY AUTOINCREMENT,"
        "pdi_Serial  TEXT,"
        "ld_LoanDate TEXT,"
        "ei_ID       INTEGER,"
        "fi_ID       INTEGER,"
        "ld_PlanDate TEXT,"
        "ld_State    INTEGER"
        ");",

        "CREATE TABLE EmployeeInfo ("
        "ei_ID   INTEGER  PRIMARY KEY AUTOINCREMENT,"
        "ei_Name TEXT"
        ");",

        "CREATE TABLE FinalUserInfo ("
        "fi_ID   INTEGER  PRIMARY KEY AUTOINCREMENT,"
        "fi_Name TEXT"
        ");",

        "CREATE TABLE PutBook ("
        "pb_ID      INTEGER  PRIMARY KEY,"
        "pdi_Serial TEXT,"
        "pb_PutDate TEXT,"
        "pb_Reason  INTEGER"
        ");",

        "CREATE TABLE OutBook ("
        "ob_ID      INTEGER  PRIMARY KEY,"
        "pdi_Serial TEXT,"
        "ob_PutDate TEXT,"
        "ob_Reason  INTEGER"
        ");",

        "CREATE TABLE CurrentStock ("
        "cs_ID      INTEGER  PRIMARY KEY,"
        "pdi_Serial TEXT,"
        "cs_Shelf   TEXT"
        ");",

        "CREATE TABLE PutBookEnum ("
        "pb_Reason   INTEGER,"
        "pbe_Name    TEXT"
        ");",

        "CREATE TABLE OutBookEnum ("
        "ob_Reason INTEGER,"
        "obe_Name TEXT"
        ");",

        "CREATE TABLE LoanDetailEnum ("
        "ld_State INTEGER,"
        "lde_Name TEXT"
        ");",

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

        ""
    };

    const QString strSTableNames[] = 
    {
        "LoanDetail",
        "ProductDetailInfo",
        "CurrentStock",
        "PutBook",
        "OutBook",
        ""
    };

//     const QString strInsertDatas[] = 
//     {
//         QObject::tr("INSERT INTO OutBookEnum(ob_Reason, obe_Name) VALUES(0, '销售');"),
//         QObject::tr("INSERT INTO OutBookEnum(ob_Reason, obe_Name) VALUES(1, '外借');"),
//         QObject::tr("INSERT INTO PutBookEnum(pb_Reason, pbe_Name) VALUES(0, '生产');"),
//         QObject::tr("INSERT INTO PutBookEnum(pb_Reason, pbe_Name) VALUES(1, '归还');"),
//         ""
//     };


    const QString strQueryLoan = 
        "SELECT pi.pi_Name, ld.pdi_Serial, DATETIME(ld.ld_LoanDate), ei.ei_Name, fi.fi_Name, "
        "ld.ld_PlanDate, lde.lde_Name FROM LoanDetail AS ld "
        "LEFT JOIN ProductDetailInfo AS pdi ON ld.pdi_Serial = pdi.pdi_Serial "
        "LEFT JOIN ProductInfo AS pi ON pi.pi_ID = pdi.pi_ID "
        "LEFT JOIN EmployeeInfo AS ei ON ld.ei_ID = ei.ei_ID "
        "LEFT JOIN FinalUserInfo AS fi ON fi.fi_ID = ld.fi_ID "
        "LEFT JOIN LoanDetailEnum AS lde ON lde.ld_State = ld.ld_State "
        "WHERE ";

    UserInfo userInfo;
};
