#include "StdAfx.h"
#include "missmodifyinfo.h"

MissModifyInfo::MissModifyInfo(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    s_changeName(0);
}

MissModifyInfo::~MissModifyInfo()
{

}

void MissModifyInfo::s_changeName( int nIndex )
{
    ui.lblName->setText(ui.cBoxName->itemText(nIndex%2));
    switch (nIndex)
    {
    case 0:
    case 1:
        ui.edtError->setEnabled(true);
        ui.lblDesc->setVisible(false);
        ui.pedtDesc->setVisible(false);
        break;
    case 2:
    case 3:
        ui.edtError->setEnabled(false);
        ui.lblDesc->setText(ui.cBoxName->itemText(nIndex));
        ui.lblDesc->setVisible(true);
        ui.pedtDesc->setVisible(true);
        break;
    }
}
