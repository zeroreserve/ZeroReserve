!include("../Common/retroshare_plugin.pri"):error("Could not include file ../Common/retroshare_plugin.pri")

CONFIG += qt \
    uic \
    qrc \
    resources

SOURCES = ZeroReserveDialog.cpp \
    ZeroReservePlugin.cpp \
    frienddetailsdialog.cpp \
    paymentdialog.cpp \
    OrderBook.cpp \
    Currency.cpp \
    p3ZeroReserveRS.cpp \
    RSZeroReserveItems.cpp \
    Payment.cpp \
    TransactionManager.cpp \
    zrdb.cpp \
    MyOrders.cpp

LIBS += -lsqlite3

DEFINES += ZR_TESTNET

HEADERS = ZeroReserveDialog.h \
    ZeroReservePlugin.h \
    frienddetailsdialog.h \
    paymentdialog.h \
    OrderBook.h \
    Currency.h \
    p3ZeroReserverRS.h \
    RSZeroReserveItems.h \
    Payment.h \
    TransactionManager.h \
    zrdb.h \
    zrtypes.h \
    MyOrders.h

FORMS = ZeroReserveDialog.ui \
    frienddetailsdialog.ui \
    paymentdialog.ui

TARGET = ZeroReserve

RESOURCES = ZeroReserve_images.qrc
