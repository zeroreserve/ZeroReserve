/********************************************************************************
** Form generated from reading UI file 'paymentdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAYMENTDIALOG_H
#define UI_PAYMENTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_PaymentDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QLineEdit *amount;
    QComboBox *currencySelector;
    QLCDNumber *lcdAvailableFunds;
    QLabel *label_3;
    QLabel *availableFundsLabel;
    QLabel *label_4;
    QLabel *label_5;

    void setupUi(QDialog *PaymentDialog)
    {
        if (PaymentDialog->objectName().isEmpty())
            PaymentDialog->setObjectName(QString::fromUtf8("PaymentDialog"));
        PaymentDialog->resize(442, 351);
        buttonBox = new QDialogButtonBox(PaymentDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 285, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(PaymentDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(30, 60, 341, 31));
        QFont font;
        font.setPointSize(12);
        label->setFont(font);
        amount = new QLineEdit(PaymentDialog);
        amount->setObjectName(QString::fromUtf8("amount"));
        amount->setGeometry(QRect(215, 236, 151, 20));
        currencySelector = new QComboBox(PaymentDialog);
        currencySelector->setObjectName(QString::fromUtf8("currencySelector"));
        currencySelector->setGeometry(QRect(215, 126, 151, 31));
        lcdAvailableFunds = new QLCDNumber(PaymentDialog);
        lcdAvailableFunds->setObjectName(QString::fromUtf8("lcdAvailableFunds"));
        lcdAvailableFunds->setGeometry(QRect(215, 186, 151, 23));
        label_3 = new QLabel(PaymentDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(60, 235, 131, 20));
        QFont font1;
        font1.setPointSize(11);
        label_3->setFont(font1);
        availableFundsLabel = new QLabel(PaymentDialog);
        availableFundsLabel->setObjectName(QString::fromUtf8("availableFundsLabel"));
        availableFundsLabel->setGeometry(QRect(60, 186, 131, 20));
        availableFundsLabel->setFont(font1);
        label_4 = new QLabel(PaymentDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(60, 130, 141, 21));
        label_4->setFont(font1);
        label_5 = new QLabel(PaymentDialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(30, 30, 271, 31));
        label_5->setFont(font);

        retranslateUi(PaymentDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), PaymentDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), PaymentDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(PaymentDialog);
    } // setupUi

    void retranslateUi(QDialog *PaymentDialog)
    {
        PaymentDialog->setWindowTitle(QApplication::translate("PaymentDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("PaymentDialog", "Error, no friend selected", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("PaymentDialog", "Amount", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        availableFundsLabel->setToolTip(QApplication::translate("PaymentDialog", "Your credit + balance. ", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        availableFundsLabel->setText(QApplication::translate("PaymentDialog", "Available Funds", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("PaymentDialog", "Currency", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("PaymentDialog", "Pay To:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PaymentDialog: public Ui_PaymentDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAYMENTDIALOG_H
