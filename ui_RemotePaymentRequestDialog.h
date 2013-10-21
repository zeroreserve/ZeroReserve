/********************************************************************************
** Form generated from reading UI file 'RemotePaymentRequestDialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REMOTEPAYMENTREQUESTDIALOG_H
#define UI_REMOTEPAYMENTREQUESTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_RemotePaymentRequestDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *payAddress;
    QLineEdit *amount;
    QLabel *label_2;
    QComboBox *currencySelector;
    QLabel *label;

    void setupUi(QDialog *RemotePaymentRequestDialog)
    {
        if (RemotePaymentRequestDialog->objectName().isEmpty())
            RemotePaymentRequestDialog->setObjectName(QString::fromUtf8("RemotePaymentRequestDialog"));
        RemotePaymentRequestDialog->resize(584, 389);
        buttonBox = new QDialogButtonBox(RemotePaymentRequestDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(70, 330, 160, 25));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        payAddress = new QLineEdit(RemotePaymentRequestDialog);
        payAddress->setObjectName(QString::fromUtf8("payAddress"));
        payAddress->setGeometry(QRect(100, 230, 411, 31));
        payAddress->setAcceptDrops(false);
        payAddress->setReadOnly(true);
        amount = new QLineEdit(RemotePaymentRequestDialog);
        amount->setObjectName(QString::fromUtf8("amount"));
        amount->setGeometry(QRect(100, 160, 191, 31));
        label_2 = new QLabel(RemotePaymentRequestDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(26, 170, 61, 20));
        currencySelector = new QComboBox(RemotePaymentRequestDialog);
        currencySelector->setObjectName(QString::fromUtf8("currencySelector"));
        currencySelector->setGeometry(QRect(310, 160, 201, 31));
        label = new QLabel(RemotePaymentRequestDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(30, 30, 501, 51));
        QFont font;
        font.setPointSize(12);
        label->setFont(font);

        retranslateUi(RemotePaymentRequestDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), RemotePaymentRequestDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), RemotePaymentRequestDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(RemotePaymentRequestDialog);
    } // setupUi

    void retranslateUi(QDialog *RemotePaymentRequestDialog)
    {
        RemotePaymentRequestDialog->setWindowTitle(QApplication::translate("RemotePaymentRequestDialog", "Dialog", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        payAddress->setToolTip(QApplication::translate("RemotePaymentRequestDialog", "<html><head/><body><p>copy this to the clipboard and send a message to the recipient</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("RemotePaymentRequestDialog", "Amount", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("RemotePaymentRequestDialog", "Enter Amount and Currency. An Address is then generated\n"
"which must be sent to the recipient by email or other means", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RemotePaymentRequestDialog: public Ui_RemotePaymentRequestDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REMOTEPAYMENTREQUESTDIALOG_H
