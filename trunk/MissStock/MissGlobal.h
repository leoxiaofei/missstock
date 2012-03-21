#pragma once

namespace MissGlobal
{
    enum PbReason{PR_Produce, PR_Return};
    enum ObReason{OR_Sell, OR_Loan};
    enum LdState{LS_Loan, LS_Return, LS_Sell};
    enum PdiType{PT_Official, PT_Demo};
    extern const QString strCreateTables[];
    //extern const QString strInsertDatas[];
    extern const QString strQueryLoan;
};
