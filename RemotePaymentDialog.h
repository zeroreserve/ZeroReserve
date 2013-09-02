#ifndef REMOTEPAYMENTDIALOG_H
#define REMOTEPAYMENTDIALOG_H

#include <QDialog>

namespace Ui {
class RemotePaymentDialog;
}

class RemotePaymentDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit RemotePaymentDialog(QWidget *parent = 0);
    ~RemotePaymentDialog();
    
private:
    Ui::RemotePaymentDialog *ui;
};

#endif // REMOTEPAYMENTDIALOG_H
