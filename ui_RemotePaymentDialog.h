/********************************************************************************
** Form generated from reading UI file 'RemotePaymentDialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REMOTEPAYMENTDIALOG_H
#define UI_REMOTEPAYMENTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_RemotePaymentDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *destination;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *currency;
    QLineEdit *amount;

    void setupUi(QDialog *RemotePaymentDialog)
    {
        if (RemotePaymentDialog->objectName().isEmpty())
            RemotePaymentDialog->setObjectName(QString::fromUtf8("RemotePaymentDialog"));
        RemotePaymentDialog->resize(664, 300);
        buttonBox = new QDialogButtonBox(RemotePaymentDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(370, 210, 161, 31));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        destination = new QLineEdit(RemotePaymentDialog);
        destination->setObjectName(QString::fromUtf8("destination"));
        destination->setGeometry(QRect(150, 110, 431, 31));
        destination->setText(QString::fromUtf8(""));
        destination->setDragEnabled(true);
        label = new QLabel(RemotePaymentDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(30, 163, 91, 21));
        label_2 = new QLabel(RemotePaymentDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(26, 114, 91, 21));
        label_3 = new QLabel(RemotePaymentDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(30, 20, 581, 31));
        QFont font;
        font.setPointSize(12);
        label_3->setFont(font);
        currency = new QLabel(RemotePaymentDialog);
        currency->setObjectName(QString::fromUtf8("currency"));
        currency->setGeometry(QRect(350, 160, 111, 31));
        QFont font1;
        font1.setPointSize(10);
        currency->setFont(font1);
        amount = new QLineEdit(RemotePaymentDialog);
        amount->setObjectName(QString::fromUtf8("amount"));
        amount->setGeometry(QRect(150, 160, 191, 31));

        retranslateUi(RemotePaymentDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), RemotePaymentDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), RemotePaymentDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(RemotePaymentDialog);
    } // setupUi

    void retranslateUi(QDialog *RemotePaymentDialog)
    {
        RemotePaymentDialog->setWindowTitle(QApplication::translate("RemotePaymentDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("RemotePaymentDialog", "Amount", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("RemotePaymentDialog", "Destination", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("RemotePaymentDialog", "Paste the destination address. The amount will appear automatically.", 0, QApplication::UnicodeUTF8));
        currency->setText(QApplication::translate("RemotePaymentDialog", "-", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RemotePaymentDialog: public Ui_RemotePaymentDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REMOTEPAYMENTDIALOG_H
