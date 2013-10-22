/********************************************************************************
** Form generated from reading UI file 'ZeroReserveDialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZERORESERVEDIALOG_H
#define UI_ZERORESERVEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QTableView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "gui/common/FriendSelectionWidget.h"

QT_BEGIN_NAMESPACE

class Ui_ZeroReserveDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *titleBarFrame;
    QHBoxLayout *horizontalLayout;
    QLabel *titleBarPixmap;
    QLabel *titleBarLabel;
    QSpacerItem *spacerItem;
    QTabWidget *ZeroReserveTabs;
    QWidget *BitcoinTab;
    QVBoxLayout *verticalLayout_4;
    QGridLayout *gridLayout;
    QLabel *label_14;
    QLabel *label;
    QTableView *asksTableView;
    QLabel *label_13;
    QLabel *label_10;
    QGridLayout *gridLayout_2;
    QLabel *label_12;
    QLineEdit *ask_amount;
    QLineEdit *ask_price;
    QLabel *label_11;
    QPushButton *askButton;
    QGridLayout *gridLayout_3;
    QLabel *label_8;
    QLineEdit *bid_price;
    QLabel *label_9;
    QLineEdit *bid_amount;
    QPushButton *bidButton;
    QTableView *myOrders;
    QLabel *label_7;
    QVBoxLayout *verticalLayout_2;
    QTableView *bidsTableView;
    QComboBox *currencySelector1;
    QLabel *testnet_warning;
    QWidget *BitcoinWalletTab;
    QHBoxLayout *BitcoinTabLayout;
    QVBoxLayout *BitcoinAddrLayout;
    QLabel *label_16;
    QTableView *MyAddresses;
    QLabel *label_17;
    QTableView *PeerAddresses;
    QVBoxLayout *BitcoinTxLogLayout;
    QLabel *label_18;
    QListView *btcTxList;
    QVBoxLayout *BitcoinDisplayLayout;
    QLabel *label_19;
    QLCDNumber *lcdNumber;
    QLabel *label_20;
    QLCDNumber *lcdNumber_2;
    QSpacerItem *verticalSpacer_2;
    QWidget *PaymentTab;
    QHBoxLayout *horizontalLayout_2;
    FriendSelectionWidget *friendSelectionWidget;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_2;
    QListWidget *paymentHistoryList;
    QVBoxLayout *verticalLayout_3;
    QComboBox *currencySelector2;
    QPushButton *remoteRequest;
    QPushButton *remotePayment;
    QSpacerItem *verticalSpacer;
    QLabel *label_15;
    QLCDNumber *lcdTheirCredit;
    QLabel *label_3;
    QLCDNumber *lcdTotalCredit;
    QLabel *label_4;
    QLCDNumber *lcdTotalDebt;
    QLabel *label_5;
    QLCDNumber *lcdtotalOutstanding;
    QLabel *label_6;
    QLCDNumber *lcdBalance;

    void setupUi(QWidget *ZeroReserveDialog)
    {
        if (ZeroReserveDialog->objectName().isEmpty())
            ZeroReserveDialog->setObjectName(QString::fromUtf8("ZeroReserveDialog"));
        ZeroReserveDialog->resize(1010, 648);
        verticalLayout = new QVBoxLayout(ZeroReserveDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        titleBarFrame = new QFrame(ZeroReserveDialog);
        titleBarFrame->setObjectName(QString::fromUtf8("titleBarFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(titleBarFrame->sizePolicy().hasHeightForWidth());
        titleBarFrame->setSizePolicy(sizePolicy);
        titleBarFrame->setFrameShape(QFrame::Box);
        titleBarFrame->setFrameShadow(QFrame::Sunken);
        horizontalLayout = new QHBoxLayout(titleBarFrame);
        horizontalLayout->setContentsMargins(2, 2, 2, 2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        titleBarPixmap = new QLabel(titleBarFrame);
        titleBarPixmap->setObjectName(QString::fromUtf8("titleBarPixmap"));
        titleBarPixmap->setPixmap(QPixmap(QString::fromUtf8("images/bitcoin.png")));

        horizontalLayout->addWidget(titleBarPixmap);

        titleBarLabel = new QLabel(titleBarFrame);
        titleBarLabel->setObjectName(QString::fromUtf8("titleBarLabel"));
        QFont font;
        font.setPointSize(12);
        titleBarLabel->setFont(font);
        titleBarLabel->setText(QString::fromUtf8("Zero Reserve"));

        horizontalLayout->addWidget(titleBarLabel);

        spacerItem = new QSpacerItem(123, 13, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(spacerItem);


        verticalLayout->addWidget(titleBarFrame);

        ZeroReserveTabs = new QTabWidget(ZeroReserveDialog);
        ZeroReserveTabs->setObjectName(QString::fromUtf8("ZeroReserveTabs"));
        BitcoinTab = new QWidget();
        BitcoinTab->setObjectName(QString::fromUtf8("BitcoinTab"));
        verticalLayout_4 = new QVBoxLayout(BitcoinTab);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        gridLayout = new QGridLayout();
        gridLayout->setContentsMargins(16, 16, 16, 16);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetNoConstraint);
        gridLayout->setHorizontalSpacing(16);
        gridLayout->setVerticalSpacing(6);
        label_14 = new QLabel(BitcoinTab);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setFont(font);
        label_14->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_14, 3, 2, 1, 1);

        label = new QLabel(BitcoinTab);
        label->setObjectName(QString::fromUtf8("label"));
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 3, 0, 1, 1);

        asksTableView = new QTableView(BitcoinTab);
        asksTableView->setObjectName(QString::fromUtf8("asksTableView"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(asksTableView->sizePolicy().hasHeightForWidth());
        asksTableView->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(asksTableView, 4, 1, 1, 1);

        label_13 = new QLabel(BitcoinTab);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        QFont font1;
        font1.setPointSize(14);
        label_13->setFont(font1);
        label_13->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_13, 0, 0, 1, 1);

        label_10 = new QLabel(BitcoinTab);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setFont(font1);
        label_10->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_10, 0, 1, 1, 1);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_12 = new QLabel(BitcoinTab);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_12, 1, 0, 1, 1);

        ask_amount = new QLineEdit(BitcoinTab);
        ask_amount->setObjectName(QString::fromUtf8("ask_amount"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(ask_amount->sizePolicy().hasHeightForWidth());
        ask_amount->setSizePolicy(sizePolicy2);

        gridLayout_2->addWidget(ask_amount, 0, 1, 1, 1);

        ask_price = new QLineEdit(BitcoinTab);
        ask_price->setObjectName(QString::fromUtf8("ask_price"));
        sizePolicy2.setHeightForWidth(ask_price->sizePolicy().hasHeightForWidth());
        ask_price->setSizePolicy(sizePolicy2);

        gridLayout_2->addWidget(ask_price, 1, 1, 1, 1);

        label_11 = new QLabel(BitcoinTab);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_11, 0, 0, 1, 1);

        askButton = new QPushButton(BitcoinTab);
        askButton->setObjectName(QString::fromUtf8("askButton"));

        gridLayout_2->addWidget(askButton, 2, 1, 1, 1);


        gridLayout->addLayout(gridLayout_2, 1, 0, 1, 1);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_8 = new QLabel(BitcoinTab);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(label_8, 0, 0, 1, 1);

        bid_price = new QLineEdit(BitcoinTab);
        bid_price->setObjectName(QString::fromUtf8("bid_price"));
        sizePolicy2.setHeightForWidth(bid_price->sizePolicy().hasHeightForWidth());
        bid_price->setSizePolicy(sizePolicy2);

        gridLayout_3->addWidget(bid_price, 1, 1, 1, 1);

        label_9 = new QLabel(BitcoinTab);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(label_9, 1, 0, 1, 1);

        bid_amount = new QLineEdit(BitcoinTab);
        bid_amount->setObjectName(QString::fromUtf8("bid_amount"));
        sizePolicy2.setHeightForWidth(bid_amount->sizePolicy().hasHeightForWidth());
        bid_amount->setSizePolicy(sizePolicy2);
        bid_amount->setMaxLength(32);

        gridLayout_3->addWidget(bid_amount, 0, 1, 1, 1);

        bidButton = new QPushButton(BitcoinTab);
        bidButton->setObjectName(QString::fromUtf8("bidButton"));

        gridLayout_3->addWidget(bidButton, 2, 1, 1, 1);


        gridLayout->addLayout(gridLayout_3, 1, 1, 1, 1);

        myOrders = new QTableView(BitcoinTab);
        myOrders->setObjectName(QString::fromUtf8("myOrders"));
        sizePolicy1.setHeightForWidth(myOrders->sizePolicy().hasHeightForWidth());
        myOrders->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(myOrders, 4, 2, 1, 1);

        label_7 = new QLabel(BitcoinTab);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setFont(font);
        label_7->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_7, 3, 1, 1, 1);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setSizeConstraint(QLayout::SetDefaultConstraint);
        bidsTableView = new QTableView(BitcoinTab);
        bidsTableView->setObjectName(QString::fromUtf8("bidsTableView"));
        sizePolicy1.setHeightForWidth(bidsTableView->sizePolicy().hasHeightForWidth());
        bidsTableView->setSizePolicy(sizePolicy1);

        verticalLayout_2->addWidget(bidsTableView);


        gridLayout->addLayout(verticalLayout_2, 4, 0, 1, 1);

        currencySelector1 = new QComboBox(BitcoinTab);
        currencySelector1->setObjectName(QString::fromUtf8("currencySelector1"));

        gridLayout->addWidget(currencySelector1, 0, 2, 1, 1);

        testnet_warning = new QLabel(BitcoinTab);
        testnet_warning->setObjectName(QString::fromUtf8("testnet_warning"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(testnet_warning->sizePolicy().hasHeightForWidth());
        testnet_warning->setSizePolicy(sizePolicy3);
        QFont font2;
        font2.setPointSize(32);
        testnet_warning->setFont(font2);

        gridLayout->addWidget(testnet_warning, 1, 2, 1, 1);


        verticalLayout_4->addLayout(gridLayout);

        ZeroReserveTabs->addTab(BitcoinTab, QString());
        BitcoinWalletTab = new QWidget();
        BitcoinWalletTab->setObjectName(QString::fromUtf8("BitcoinWalletTab"));
        BitcoinTabLayout = new QHBoxLayout(BitcoinWalletTab);
        BitcoinTabLayout->setObjectName(QString::fromUtf8("BitcoinTabLayout"));
        BitcoinAddrLayout = new QVBoxLayout();
        BitcoinAddrLayout->setObjectName(QString::fromUtf8("BitcoinAddrLayout"));
        label_16 = new QLabel(BitcoinWalletTab);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        BitcoinAddrLayout->addWidget(label_16);

        MyAddresses = new QTableView(BitcoinWalletTab);
        MyAddresses->setObjectName(QString::fromUtf8("MyAddresses"));

        BitcoinAddrLayout->addWidget(MyAddresses);

        label_17 = new QLabel(BitcoinWalletTab);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        BitcoinAddrLayout->addWidget(label_17);

        PeerAddresses = new QTableView(BitcoinWalletTab);
        PeerAddresses->setObjectName(QString::fromUtf8("PeerAddresses"));

        BitcoinAddrLayout->addWidget(PeerAddresses);


        BitcoinTabLayout->addLayout(BitcoinAddrLayout);

        BitcoinTxLogLayout = new QVBoxLayout();
        BitcoinTxLogLayout->setObjectName(QString::fromUtf8("BitcoinTxLogLayout"));
        label_18 = new QLabel(BitcoinWalletTab);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        BitcoinTxLogLayout->addWidget(label_18);

        btcTxList = new QListView(BitcoinWalletTab);
        btcTxList->setObjectName(QString::fromUtf8("btcTxList"));

        BitcoinTxLogLayout->addWidget(btcTxList);


        BitcoinTabLayout->addLayout(BitcoinTxLogLayout);

        BitcoinDisplayLayout = new QVBoxLayout();
        BitcoinDisplayLayout->setObjectName(QString::fromUtf8("BitcoinDisplayLayout"));
        label_19 = new QLabel(BitcoinWalletTab);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        BitcoinDisplayLayout->addWidget(label_19);

        lcdNumber = new QLCDNumber(BitcoinWalletTab);
        lcdNumber->setObjectName(QString::fromUtf8("lcdNumber"));
        lcdNumber->setStyleSheet(QString::fromUtf8("background-color: rgb(59, 59, 59);"));

        BitcoinDisplayLayout->addWidget(lcdNumber);

        label_20 = new QLabel(BitcoinWalletTab);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        BitcoinDisplayLayout->addWidget(label_20);

        lcdNumber_2 = new QLCDNumber(BitcoinWalletTab);
        lcdNumber_2->setObjectName(QString::fromUtf8("lcdNumber_2"));
        lcdNumber_2->setStyleSheet(QString::fromUtf8("background-color: rgb(59, 59, 59);"));

        BitcoinDisplayLayout->addWidget(lcdNumber_2);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        BitcoinDisplayLayout->addItem(verticalSpacer_2);


        BitcoinTabLayout->addLayout(BitcoinDisplayLayout);

        ZeroReserveTabs->addTab(BitcoinWalletTab, QString());
        PaymentTab = new QWidget();
        PaymentTab->setObjectName(QString::fromUtf8("PaymentTab"));
        horizontalLayout_2 = new QHBoxLayout(PaymentTab);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        friendSelectionWidget = new FriendSelectionWidget(PaymentTab);
        friendSelectionWidget->setObjectName(QString::fromUtf8("friendSelectionWidget"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(4);
        sizePolicy4.setHeightForWidth(friendSelectionWidget->sizePolicy().hasHeightForWidth());
        friendSelectionWidget->setSizePolicy(sizePolicy4);
        friendSelectionWidget->setMinimumSize(QSize(200, 0));
        friendSelectionWidget->setMaximumSize(QSize(300, 16777215));
        friendSelectionWidget->setSizeIncrement(QSize(220, 0));
        friendSelectionWidget->setBaseSize(QSize(200, 0));
        friendSelectionWidget->setContextMenuPolicy(Qt::CustomContextMenu);

        horizontalLayout_2->addWidget(friendSelectionWidget);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        label_2 = new QLabel(PaymentTab);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(label_2);

        paymentHistoryList = new QListWidget(PaymentTab);
        paymentHistoryList->setObjectName(QString::fromUtf8("paymentHistoryList"));

        verticalLayout_5->addWidget(paymentHistoryList);


        horizontalLayout_2->addLayout(verticalLayout_5);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        currencySelector2 = new QComboBox(PaymentTab);
        currencySelector2->setObjectName(QString::fromUtf8("currencySelector2"));

        verticalLayout_3->addWidget(currencySelector2);

        remoteRequest = new QPushButton(PaymentTab);
        remoteRequest->setObjectName(QString::fromUtf8("remoteRequest"));

        verticalLayout_3->addWidget(remoteRequest);

        remotePayment = new QPushButton(PaymentTab);
        remotePayment->setObjectName(QString::fromUtf8("remotePayment"));

        verticalLayout_3->addWidget(remotePayment);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        label_15 = new QLabel(PaymentTab);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_15);

        lcdTheirCredit = new QLCDNumber(PaymentTab);
        lcdTheirCredit->setObjectName(QString::fromUtf8("lcdTheirCredit"));
        lcdTheirCredit->setStyleSheet(QString::fromUtf8("background-color: rgb(59, 59, 59);"));

        verticalLayout_3->addWidget(lcdTheirCredit);

        label_3 = new QLabel(PaymentTab);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_3);

        lcdTotalCredit = new QLCDNumber(PaymentTab);
        lcdTotalCredit->setObjectName(QString::fromUtf8("lcdTotalCredit"));
        lcdTotalCredit->setStyleSheet(QString::fromUtf8("background-color: rgb(59, 59, 59);"));

        verticalLayout_3->addWidget(lcdTotalCredit);

        label_4 = new QLabel(PaymentTab);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_4);

        lcdTotalDebt = new QLCDNumber(PaymentTab);
        lcdTotalDebt->setObjectName(QString::fromUtf8("lcdTotalDebt"));
        lcdTotalDebt->setStyleSheet(QString::fromUtf8("background-color: rgb(59, 59, 59);"));

        verticalLayout_3->addWidget(lcdTotalDebt);

        label_5 = new QLabel(PaymentTab);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_5);

        lcdtotalOutstanding = new QLCDNumber(PaymentTab);
        lcdtotalOutstanding->setObjectName(QString::fromUtf8("lcdtotalOutstanding"));
        lcdtotalOutstanding->setStyleSheet(QString::fromUtf8("background-color: rgb(59, 59, 59);"));

        verticalLayout_3->addWidget(lcdtotalOutstanding);

        label_6 = new QLabel(PaymentTab);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_6);

        lcdBalance = new QLCDNumber(PaymentTab);
        lcdBalance->setObjectName(QString::fromUtf8("lcdBalance"));
        lcdBalance->setStyleSheet(QString::fromUtf8("background-color: rgb(59, 59, 59);"));

        verticalLayout_3->addWidget(lcdBalance);


        horizontalLayout_2->addLayout(verticalLayout_3);

        ZeroReserveTabs->addTab(PaymentTab, QString());

        verticalLayout->addWidget(ZeroReserveTabs);


        retranslateUi(ZeroReserveDialog);

        ZeroReserveTabs->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(ZeroReserveDialog);
    } // setupUi

    void retranslateUi(QWidget *ZeroReserveDialog)
    {
        titleBarPixmap->setText(QString());
        label_14->setText(QApplication::translate("ZeroReserveDialog", "My Orders", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ZeroReserveDialog", "Bids", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("ZeroReserveDialog", "Sell Bitcoins", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("ZeroReserveDialog", "Buy Bitcoins", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("ZeroReserveDialog", "Min Price:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ask_price->setToolTip(QApplication::translate("ZeroReserveDialog", "Leave empty to create a market order", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_11->setText(QApplication::translate("ZeroReserveDialog", "Amount:", 0, QApplication::UnicodeUTF8));
        askButton->setText(QApplication::translate("ZeroReserveDialog", "Sell", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("ZeroReserveDialog", "Amount:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        bid_price->setToolTip(QApplication::translate("ZeroReserveDialog", "Leave empty to create a market order.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_9->setText(QApplication::translate("ZeroReserveDialog", "Max Price:", 0, QApplication::UnicodeUTF8));
        bidButton->setText(QApplication::translate("ZeroReserveDialog", "Buy", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("ZeroReserveDialog", "Asks", 0, QApplication::UnicodeUTF8));
        testnet_warning->setText(QString());
        ZeroReserveTabs->setTabText(ZeroReserveTabs->indexOf(BitcoinTab), QApplication::translate("ZeroReserveDialog", "Bitcoin Market", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("ZeroReserveDialog", "My Addresses", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("ZeroReserveDialog", "Peer Addresses", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("ZeroReserveDialog", "Transactions", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("ZeroReserveDialog", "Balance", 0, QApplication::UnicodeUTF8));
        label_20->setText(QApplication::translate("ZeroReserveDialog", "Unconfirmed", 0, QApplication::UnicodeUTF8));
        ZeroReserveTabs->setTabText(ZeroReserveTabs->indexOf(BitcoinWalletTab), QApplication::translate("ZeroReserveDialog", "Bitcoin Wallet", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ZeroReserveDialog", "Payment History", 0, QApplication::UnicodeUTF8));
        remoteRequest->setText(QApplication::translate("ZeroReserveDialog", "Remote Request", 0, QApplication::UnicodeUTF8));
        remotePayment->setText(QApplication::translate("ZeroReserveDialog", "Remote Payment", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("ZeroReserveDialog", "Their Total Credit", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ZeroReserveDialog", "Total Credit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        lcdTotalCredit->setToolTip(QApplication::translate("ZeroReserveDialog", "The amount you can borrow from your friends", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_4->setText(QApplication::translate("ZeroReserveDialog", "Total Debt", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        lcdTotalDebt->setToolTip(QApplication::translate("ZeroReserveDialog", "The amount you borrowed from your friends", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_5->setText(QApplication::translate("ZeroReserveDialog", "Total Outstanding", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        lcdtotalOutstanding->setToolTip(QApplication::translate("ZeroReserveDialog", "the amount your friends owe you", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_6->setText(QApplication::translate("ZeroReserveDialog", "Balance", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        lcdBalance->setToolTip(QApplication::translate("ZeroReserveDialog", "Outstanding claims minus debt", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        ZeroReserveTabs->setTabText(ZeroReserveTabs->indexOf(PaymentTab), QApplication::translate("ZeroReserveDialog", "Payment", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ZeroReserveDialog);
    } // retranslateUi

};

namespace Ui {
    class ZeroReserveDialog: public Ui_ZeroReserveDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZERORESERVEDIALOG_H
