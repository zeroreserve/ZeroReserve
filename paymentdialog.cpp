#include "paymentdialog.h"
#include "ui_paymentdialog.h"

PaymentDialog::PaymentDialog(QWidget *parent, const std::string & peername ) :
    QDialog(parent),
    ui(new Ui::PaymentDialog)
{
    ui->setupUi(this);
    ui->label->setText( QString::fromUtf8( peername.c_str() ) );
}

PaymentDialog::~PaymentDialog()
{
    delete ui;
}
