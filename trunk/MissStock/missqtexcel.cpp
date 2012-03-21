#include "StdAfx.h"
#include "missqtexcel.h"
#include <QAxObject>

class MissQtExcel::MissQtExcelImpl
{
public:
    QAxObject* m_App;
    QAxObject* m_Books;
    QAxObject* m_Book;
    QAxObject* m_Sheets;
    QAxObject* m_Sheet;
    QAxObject* m_Range;
};

MissQtExcel::MissQtExcel(QObject *parent)
    : QObject(parent),
    m_pImpl(new MissQtExcelImpl)
{

}

MissQtExcel::~MissQtExcel()
{

}

bool MissQtExcel::Init()
{
    m_pImpl->m_App    = new QAxObject( "Excel.Application", this );
    m_pImpl->m_Books  = m_pImpl->m_App->querySubObject( "Workbooks" );
    return m_pImpl->m_Books != NULL;
}

bool MissQtExcel::Open( const QString& strFileName )
{
    m_pImpl->m_Book   = m_pImpl->m_Books->querySubObject( "Open(const QString&)", strFileName );
    m_pImpl->m_Sheets = m_pImpl->m_Book->querySubObject( "Worksheets" );
    OpenSheet(1);
    return true;
}

bool MissQtExcel::NewBook()
{
    m_pImpl->m_Book   = m_pImpl->m_Books->querySubObject( "Add(const QString&)", "");
    m_pImpl->m_Sheets = m_pImpl->m_Book->querySubObject( "Worksheets" );
    OpenSheet(1);
    return true;
}

bool MissQtExcel::Save( const QString & strFileName )
{
    return true;
}

void MissQtExcel::ShowExcel()
{
    m_pImpl->m_App->dynamicCall("SetVisible(bool)", true);
    m_pImpl->m_App->dynamicCall("SetUserControl(bool)", true);
}

void MissQtExcel::OpenSheet( short sNo )
{
    m_pImpl->m_Sheet = m_pImpl->m_Sheets->querySubObject("Item(short)", sNo);
}

void MissQtExcel::OpenSheet( const QString& strSheetName )
{
    m_pImpl->m_Sheet = m_pImpl->m_Sheets->querySubObject("Item(QString)", strSheetName);
}

int MissQtExcel::GetUsedMaxRowCount()
{
    QAxObject* range = m_pImpl->m_Sheet->querySubObject("UsedRange()");
    range = range->querySubObject("Rows()");
    return range->dynamicCall("Count()").toInt();
}

int MissQtExcel::GetUsedMaxColumnCount()
{
    QAxObject* range = m_pImpl->m_Sheet->querySubObject("UsedRange()");
    range = range->querySubObject("Columns()");
    return range->dynamicCall("Count()").toInt();
}

QString MissQtExcel::GetValue()
{
    return m_pImpl->m_Range->dynamicCall("Value2()").toString();
}

QString MissQtExcel::GetText()
{
    return m_pImpl->m_Range->dynamicCall("Text()").toString();
}

void MissQtExcel::SetValue( const QVariant& strValue )
{
    m_pImpl->m_Range->dynamicCall("SetValue(QVariant)", strValue);
}

void MissQtExcel::SetValue( int nValue )
{
    m_pImpl->m_Range->dynamicCall("SetValue(int)", nValue);
}

void MissQtExcel::SetRange( const QString& strCell )
{
    m_pImpl->m_Range = m_pImpl->m_Sheet->querySubObject("Range(QString,QString)",strCell,strCell);
}

void MissQtExcel::SetRange( const QString& strCellBegin, const QString& strCellEnd )
{
    m_pImpl->m_Range = m_pImpl->m_Sheet->querySubObject("Range(QString,QString)",strCellBegin, strCellEnd);
}

QString MissQtExcel::GetCellName( int nRow, int nColumn )
{
    Q_ASSERT(nRow>0);
    Q_ASSERT(nColumn>0);
    Q_ASSERT(nColumn<256);
    QString strRet = GetColumnName(nColumn);
    strRet += tr("%1").arg(nRow);
    return strRet;
}

QString MissQtExcel::GetColumnName( int nColumn )
{
    Q_ASSERT(nColumn>0);
    Q_ASSERT(nColumn<256);
    QString strRet;
    if (nColumn > 26)
    {
        strRet += ('A' + (nColumn - 1) / 26 - 1);
    }
    strRet += ('A' + (nColumn - 1) % 26 );
    return strRet;
}

void MissQtExcel::Merge( bool bAcross /*= false*/ )
{
    m_pImpl->m_Range->dynamicCall("Merge(int)",bAcross);
    m_pImpl->m_Range->dynamicCall("SetHorizontalAlignment(int)",3);
}

void MissQtExcel::AutoFit( bool bRow /*= false*/ )
{
    QAxObject* cols = bRow ? m_pImpl->m_Range->querySubObject("EntireRow()") :
        m_pImpl->m_Range->querySubObject("EntireColumn()");
    cols->dynamicCall("AutoFit()");
}

//Ä¬ÈÏ£½1,¾ÓÖÐ£½-4108,×ó£½-4131,ÓÒ£½-4152
void MissQtExcel::SetHorizontalAlignment( int nType /*= -4108*/ )
{
    m_pImpl->m_Range->dynamicCall("SetHorizontalAlignment(QVariant)",nType);
}

//Ä¬ÈÏ£½2,¾ÓÖÐ£½-4108,×ó£½-4160,ÓÒ£½-4107
void MissQtExcel::SetVerticalAlignment( int nType /*= -4108*/ )
{
    m_pImpl->m_Range->dynamicCall("SetVerticalAlignment(QVariant)",nType);
}
