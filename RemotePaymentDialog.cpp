#include "RemotePaymentDialog.h"
#include "ui_RemotePaymentDialog.h"

RemotePaymentDialog::RemotePaymentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemotePaymentDialog)
{
    ui->setupUi(this);
}

RemotePaymentDialog::~RemotePaymentDialog()
{
    delete ui;
}
