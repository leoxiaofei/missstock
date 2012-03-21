#ifndef MISSMODIFYSHELF_H
#define MISSMODIFYSHELF_H

#include <QDialog>
#include "ui_missmodifyshelf.h"

class MissModifyShelf : public QDialog
{
    Q_OBJECT

public:
    MissModifyShelf(QWidget *parent = 0);
    ~MissModifyShelf();

private:
    Ui::MissModifyShelf ui;
};

#endif // MISSMODIFYSHELF_H
