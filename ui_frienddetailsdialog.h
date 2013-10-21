/********************************************************************************
** Form generated from reading UI file 'frienddetailsdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRIENDDETAILSDIALOG_H
#define UI_FRIENDDETAILSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_FriendDetailsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QComboBox *currencySelector;
    QLabel *label_2;
    QDoubleSpinBox *creditSpinBox;
    QLCDNumber *balance;
    QLabel *label_4;
    QLabel *label_3;
    QLCDNumber *yourCredit;
    QLabel *label_6;

    void setupUi(QDialog *FriendDetailsDialog)
    {
        if (FriendDetailsDialog->objectName().isEmpty())
            FriendDetailsDialog->setObjectName(QString::fromUtf8("FriendDetailsDialog"));
        FriendDetailsDialog->resize(405, 364);
        buttonBox = new QDialogButtonBox(FriendDetailsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(40, 300, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(FriendDetailsDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 361, 31));
        QFont font;
        font.setPointSize(14);
        label->setFont(font);
        currencySelector = new QComboBox(FriendDetailsDialog);
        currencySelector->setObjectName(QString::fromUtf8("currencySelector"));
        currencySelector->setGeometry(QRect(190, 61, 191, 31));
        label_2 = new QLabel(FriendDetailsDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(40, 75, 131, 20));
        QFont font1;
        font1.setPointSize(11);
        label_2->setFont(font1);
        creditSpinBox = new QDoubleSpinBox(FriendDetailsDialog);
        creditSpinBox->setObjectName(QString::fromUtf8("creditSpinBox"));
        creditSpinBox->setGeometry(QRect(190, 110, 191, 31));
        creditSpinBox->setDecimals(2);
        creditSpinBox->setMaximum(1e+06);
        balance = new QLCDNumber(FriendDetailsDialog);
        balance->setObjectName(QString::fromUtf8("balance"));
        balance->setGeometry(QRect(190, 160, 191, 31));
        label_4 = new QLabel(FriendDetailsDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(40, 170, 91, 21));
        label_4->setFont(font1);
        label_3 = new QLabel(FriendDetailsDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(40, 120, 131, 21));
        label_3->setFont(font1);
        yourCredit = new QLCDNumber(FriendDetailsDialog);
        yourCredit->setObjectName(QString::fromUtf8("yourCredit"));
        yourCredit->setGeometry(QRect(190, 220, 191, 31));
        label_6 = new QLabel(FriendDetailsDialog);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(40, 220, 131, 31));
        label_6->setFont(font1);

        retranslateUi(FriendDetailsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), FriendDetailsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), FriendDetailsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(FriendDetailsDialog);
    } // setupUi

    void retranslateUi(QDialog *FriendDetailsDialog)
    {
        FriendDetailsDialog->setWindowTitle(QApplication::translate("FriendDetailsDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("FriendDetailsDialog", "Error: No id", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("FriendDetailsDialog", "Currency", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("FriendDetailsDialog", "Balance", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("FriendDetailsDialog", "Credit", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("FriendDetailsDialog", "Your Credit", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FriendDetailsDialog: public Ui_FriendDetailsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRIENDDETAILSDIALOG_H
