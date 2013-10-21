/********************************************************************************
** Form generated from reading UI file 'ZeroReserveConfig.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZERORESERVECONFIG_H
#define UI_ZERORESERVECONFIG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DBConfig
{
public:
    QLabel *label;
    QPushButton *TxLogPathButton;

    void setupUi(QWidget *DBConfig)
    {
        if (DBConfig->objectName().isEmpty())
            DBConfig->setObjectName(QString::fromUtf8("DBConfig"));
        DBConfig->resize(407, 305);
        label = new QLabel(DBConfig);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(70, 90, 221, 41));
        TxLogPathButton = new QPushButton(DBConfig);
        TxLogPathButton->setObjectName(QString::fromUtf8("TxLogPathButton"));
        TxLogPathButton->setGeometry(QRect(104, 130, 131, 31));

        retranslateUi(DBConfig);

        QMetaObject::connectSlotsByName(DBConfig);
    } // setupUi

    void retranslateUi(QWidget *DBConfig)
    {
        DBConfig->setWindowTitle(QApplication::translate("DBConfig", "Dialog", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        DBConfig->setToolTip(QApplication::translate("DBConfig", "<html><head/><body><p>This should be located on a separate device, so if you lose your book, you can recreate it from a backup and this log.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label->setToolTip(QApplication::translate("DBConfig", "<html><head/><body><p>It is advisable to place the transaction log (TX log) on a separate device - maybe a USB stick or a memory card. That way you can always re-create your book.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("DBConfig", "Set the transaction log path", 0, QApplication::UnicodeUTF8));
        TxLogPathButton->setText(QApplication::translate("DBConfig", "TX log path...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DBConfig: public Ui_DBConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZERORESERVECONFIG_H
