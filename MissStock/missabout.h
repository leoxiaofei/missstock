#ifndef MISSABOUT_H
#define MISSABOUT_H

#include <QDialog>
#include "ui_missabout.h"

class MissAbout : public QDialog
{
    Q_OBJECT

public:
    MissAbout(QWidget *parent = 0);
    ~MissAbout();

private:
    Ui::MissAbout ui;
};

#endif // MISSABOUT_H
