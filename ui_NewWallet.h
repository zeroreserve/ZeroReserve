/********************************************************************************
** Form generated from reading UI file 'NewWallet.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWWALLET_H
#define UI_NEWWALLET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_NewWallet
{
public:
    QRadioButton *brainWallet;
    QRadioButton *importElectrum;
    QRadioButton *newElectrum;
    QDialogButtonBox *buttonBox;
    QLineEdit *seed;
    QLabel *label;

    void setupUi(QDialog *NewWallet)
    {
        if (NewWallet->objectName().isEmpty())
            NewWallet->setObjectName(QString::fromUtf8("NewWallet"));
        NewWallet->resize(571, 247);
        brainWallet = new QRadioButton(NewWallet);
        brainWallet->setObjectName(QString::fromUtf8("brainWallet"));
        brainWallet->setGeometry(QRect(40, 120, 100, 21));
        importElectrum = new QRadioButton(NewWallet);
        importElectrum->setObjectName(QString::fromUtf8("importElectrum"));
        importElectrum->setGeometry(QRect(40, 160, 191, 21));
        newElectrum = new QRadioButton(NewWallet);
        newElectrum->setObjectName(QString::fromUtf8("newElectrum"));
        newElectrum->setGeometry(QRect(40, 200, 171, 21));
        buttonBox = new QDialogButtonBox(NewWallet);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(330, 200, 160, 25));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        seed = new QLineEdit(NewWallet);
        seed->setObjectName(QString::fromUtf8("seed"));
        seed->setGeometry(QRect(30, 70, 511, 31));
        label = new QLabel(NewWallet);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(40, 30, 331, 21));
        QFont font;
        font.setPointSize(12);
        label->setFont(font);

        retranslateUi(NewWallet);
        QObject::connect(buttonBox, SIGNAL(accepted()), NewWallet, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), NewWallet, SLOT(reject()));

        QMetaObject::connectSlotsByName(NewWallet);
    } // setupUi

    void retranslateUi(QDialog *NewWallet)
    {
        NewWallet->setWindowTitle(QApplication::translate("NewWallet", "Dialog", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        brainWallet->setToolTip(QApplication::translate("NewWallet", "compatible to www.brainwallet.org - a simple sha256 hash", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        brainWallet->setText(QApplication::translate("NewWallet", "Brain Wallet", 0, QApplication::UnicodeUTF8));
        importElectrum->setText(QApplication::translate("NewWallet", "Import Electrum Seed", 0, QApplication::UnicodeUTF8));
        newElectrum->setText(QApplication::translate("NewWallet", "New Electrum Seed", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("NewWallet", "Enter a seed or Pass Phrase", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class NewWallet: public Ui_NewWallet {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWWALLET_H
