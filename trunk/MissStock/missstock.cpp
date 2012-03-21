#include "stdafx.h"
#include "missstock.h"
#include "missputindlg.h"
#include "missputoutdlg.h"

#include <QSqlRelationalTableModel>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlRecord>

#include "MissGlobal.h"
#include "missstockquery.h"
#include "missloanquery.h"
#include "misshistoryquery.h"
#include "missabout.h"
#include "missqtexcel.h"
#include "missmodifyinfo.h"

class MissStock::MissStockImpl
{
public:
    MissStockImpl(QWidget* parent):
      m_tableModel(parent),
      m_pCurretOption(NULL){};
    QSqlQueryModel m_tableModel;
    QWidget*       m_pCurretOption;
};

MissStock::MissStock(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags),
    m_pImpl(new MissStockImpl(this))
{
    ui.setupUi(this);
    ui.tvShow->setModel(&m_pImpl->m_tableModel);
    s_actionCurrentStock();
}

MissStock::~MissStock()
{

}

void MissStock::s_actionPutin()
{
    MissPutinDlg dlg(this);
    dlg.exec();

}

void MissStock::s_actionPutout()
{
    MissPutoutDlg dlg(this);
    dlg.exec();
}

void MissStock::s_actionCurrentStock()
{
    clearQueryOption();
    addQueryOption(new MissStockQuery(ui.wgQueryOption));

}

void MissStock::s_actionLoanQuery()
{
    clearQueryOption();
    addQueryOption(new MissLoanQuery(ui.wgQueryOption));
}

void MissStock::s_actionHistoryQuery()
{
    clearQueryOption();
    addQueryOption(new MissHistoryQuery(ui.wgQueryOption));
}

void MissStock::addQueryOption( QWidget* pOption )
{
    m_pImpl->m_pCurretOption = pOption;
    ui.glQueryOption->addWidget(m_pImpl->m_pCurretOption,0,0,1,1);
    connect(pOption,SIGNAL(sg_QueryDB(const QSqlQuery&,const std::vector<QString>&)),
        this,SLOT(s_optionQuery(const QSqlQuery&,const std::vector<QString>&)));
}

void MissStock::clearQueryOption()
{
    if (m_pImpl->m_pCurretOption != NULL)
    {
        ui.glQueryOption->removeWidget(m_pImpl->m_pCurretOption);
        delete m_pImpl->m_pCurretOption;
        m_pImpl->m_pCurretOption = NULL;
    }
}

void MissStock::s_optionQuery( const QSqlQuery& sqlQuery ,const std::vector<QString>& vecHeader)
{
    m_pImpl->m_tableModel.setQuery(sqlQuery);
    if (m_pImpl->m_tableModel.lastError().isValid())
    {
        qDebug() << m_pImpl->m_tableModel.lastError();
        return;
    }
    for (std::vector<QString>::size_type ix = 0; ix != vecHeader.size(); ++ix)
    {
        m_pImpl->m_tableModel.setHeaderData(ix, Qt::Horizontal, vecHeader[ix]);
    }
    ui.tvShow->resizeColumnsToContents();
}

void MissStock::s_actionAbout()
{
    MissAbout ().exec();
    
}

void MissStock::s_actionExportExcel()
{
    int nRowCount = m_pImpl->m_tableModel.rowCount();
    int nColCount = m_pImpl->m_tableModel.columnCount();
    if( nRowCount != 0 )
    {
        MissQtExcel  db(this);
        MissQtExcel* excel = &db;//new MissQtExcel(this);
        if(excel->Init())
        {
            excel->NewBook();
            for (int ix = 0; ix < nColCount; ++ix)
            {
                excel->SetRange(MissQtExcel::GetCellName(1,ix + 1));
                excel->SetValue(m_pImpl->m_tableModel.headerData(ix, Qt::Horizontal));
                excel->SetHorizontalAlignment();
            }
            QSqlRecord record;
            for (int ir = 0; ir < nRowCount; ++ir)
            {
                record = m_pImpl->m_tableModel.record(ir);
                for (int ic = 0; ic < nColCount; ++ic)
                {
                    excel->SetRange(MissQtExcel::GetCellName(ir + 2,ic + 1));
                    excel->SetValue(record.value(ic));
                }
            }
            excel->SetRange(MissQtExcel::GetCellName(1,1),MissQtExcel::GetCellName(nRowCount,nColCount));
            excel->AutoFit();
            excel->ShowExcel();
        }
        else
        {
            QMessageBox::information(this, tr("抱歉"), tr("初始化Excel失败，请重新安装MS Office."));
        }
    }
}

void MissStock::s_copyModelData( QModelIndex index )
{
    QString strData = m_pImpl->m_tableModel.data(index).toString();
    QClipboard *board = QApplication::clipboard();  
    board->setText(strData);  
}

void MissStock::s_actionModifyInfo()
{
    MissModifyInfo ().exec();
}

void MissStock::s_actionModifyShelf()
{
    MissModifyShelf ().exec();
}

