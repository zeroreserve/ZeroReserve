#ifndef PAYMENTDIALOG_H
#define PAYMENTDIALOG_H

#include <QDialog>
#include <string>

namespace Ui {
    class PaymentDialog;
}

class PaymentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PaymentDialog(QWidget *parent = 0, const std::string & peername = "Error, no friend selected");
    ~PaymentDialog();

private:
    Ui::PaymentDialog *ui;
};

#endif // PAYMENTDIALOG_H
