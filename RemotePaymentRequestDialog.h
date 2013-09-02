#ifndef REMOTEPAYMENTREQUESTDIALOG_H
#define REMOTEPAYMENTREQUESTDIALOG_H

#include <QDialog>

namespace Ui {
class RemotePaymentRequestDialog;
}

class RemotePaymentRequestDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit RemotePaymentRequestDialog(QWidget *parent = 0);
    ~RemotePaymentRequestDialog();

private slots:
    void currencySelected( QString currency );
    void amountEntered(QString amount );
    void sendRequest();

private:
    QString getPayAddress( QString amount, QString currency );
    
private:
    Ui::RemotePaymentRequestDialog *ui;
};

#endif // REMOTEPAYMENTREQUESTDIALOG_H
