!include("../Common/retroshare_plugin.pri"):error("Could not include file ../Common/retroshare_plugin.pri")

# Use bogus currency. If Testnet is a compile time option of the Bitcoin code, set it there, too
DEFINES += ZR_TESTNET

# One of those must be defined
ZR_BITCOIN = ZR_LIBBITCOIN
# ZR_BITCOIN = ZR_DUMMYBITCOIN


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
    MyOrders.cpp \
    Credit.cpp \
    dbconfig.cpp \
    Router.cpp \
    TraceRouter.cpp \
    RemotePaymentDialog.cpp \
    RemotePaymentRequestDialog.cpp \
    RSZRRemoteItems.cpp \
    TmRemoteCoordinator.cpp \
    TmLocalCoordinator.cpp \
    TmLocalCohorte.cpp \
    TmRemoteCohorte.cpp

LIBS += -lsqlite3

win32 {
    INCLUDEPATH += ../../../libsqlite ../../../boost
}


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
    MyOrders.h \
    Credit.h \
    dbconfig.h \
    Router.h \
    TraceRouter.h \
    RemotePaymentDialog.h \
    RemotePaymentRequestDialog.h \
    RSZRRemoteItems.h \
    TmRemoteCoordinator.h \
    TmLocalCoordinator.h \
    TmLocalCohorte.h \
    TmRemoteCohorte.h \
    ZRBitcoin.h \
    ZrLibBitcoin.h

FORMS = ZeroReserveDialog.ui \
    frienddetailsdialog.ui \
    paymentdialog.ui \
    ZeroReserveConfig.ui \
    RemotePaymentDialog.ui \
    RemotePaymentRequestDialog.ui

TARGET = ZeroReserve

RESOURCES = ZeroReserve_images.qrc


contains(ZR_BITCOIN, ZR_LIBBITCOIN) {
    HEADERS += ZrLibBitcoin.h
    SOURCES += ZrLibBitcoin.cpp
    QMAKE_CXXFLAGS += -std=c++11
    LIBS += -lbitcoin
}

contains(ZR_BITCOIN, ZR_DUMMYBITCOIN) {
    HEADERS += ZrDummyBitcoin.h
    SOURCES += ZrDummyBitcoin.cpp
}

