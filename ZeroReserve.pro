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
    TmRemoteCohorte.h

FORMS = ZeroReserveDialog.ui \
    frienddetailsdialog.ui \
    paymentdialog.ui \
    ZeroReserveConfig.ui \
    RemotePaymentDialog.ui \
    RemotePaymentRequestDialog.ui

TARGET = ZeroReserve

RESOURCES = ZeroReserve_images.qrc
