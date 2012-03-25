#pragma once

namespace MissGlobal
{
    enum PbReason{PR_Produce, PR_Return};
    enum ObReason{OR_Sell, OR_Loan};
    enum LdState{LS_Loan, LS_Return, LS_Sell};
    enum PdiType{PT_Official, PT_Demo};
    extern const QString strCreateTables[];
    extern const QString strSTableNames[];
    extern const QString strQueryLoan;

    struct UserInfo 
    {
        int     nUserID;
        QString strRight;
    };

    extern UserInfo userInfo;
};
