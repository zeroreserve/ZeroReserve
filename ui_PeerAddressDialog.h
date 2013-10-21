/********************************************************************************
** Form generated from reading UI file 'PeerAddressDialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PEERADDRESSDIALOG_H
#define UI_PEERADDRESSDIALOG_H

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

class Ui_PeerAddressDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QLineEdit *NickName;
    QLabel *label_2;
    QLineEdit *peerAddress;
    QLabel *label_3;

    void setupUi(QDialog *PeerAddressDialog)
    {
        if (PeerAddressDialog->objectName().isEmpty())
            PeerAddressDialog->setObjectName(QString::fromUtf8("PeerAddressDialog"));
        PeerAddressDialog->resize(428, 282);
        buttonBox = new QDialogButtonBox(PeerAddressDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(120, 210, 261, 41));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(PeerAddressDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(30, 66, 121, 20));
        NickName = new QLineEdit(PeerAddressDialog);
        NickName->setObjectName(QString::fromUtf8("NickName"));
        NickName->setGeometry(QRect(30, 90, 351, 31));
        label_2 = new QLabel(PeerAddressDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(30, 140, 121, 16));
        peerAddress = new QLineEdit(PeerAddressDialog);
        peerAddress->setObjectName(QString::fromUtf8("peerAddress"));
        peerAddress->setGeometry(QRect(30, 160, 351, 31));
        label_3 = new QLabel(PeerAddressDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(30, 20, 341, 31));
        QFont font;
        font.setPointSize(12);
        label_3->setFont(font);

        retranslateUi(PeerAddressDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), PeerAddressDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), PeerAddressDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(PeerAddressDialog);
    } // setupUi

    void retranslateUi(QDialog *PeerAddressDialog)
    {
        PeerAddressDialog->setWindowTitle(QApplication::translate("PeerAddressDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("PeerAddressDialog", "Nickname", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("PeerAddressDialog", "Bitcoin Address", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("PeerAddressDialog", "Peer Bitcoin Address", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PeerAddressDialog: public Ui_PeerAddressDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PEERADDRESSDIALOG_H
