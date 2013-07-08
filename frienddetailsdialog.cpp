#include "frienddetailsdialog.h"
#include "ui_frienddetailsdialog.h"

#include <QString>

FriendDetailsDialog::FriendDetailsDialog(QWidget *parent, const std::string & uid ) :
    QDialog(parent),
    ui(new Ui::FriendDetailsDialog)
{
    ui->setupUi(this);
    ui->label->setText( QString::fromUtf8( uid.c_str() ) );
}

FriendDetailsDialog::~FriendDetailsDialog()
{
    delete ui;
}
