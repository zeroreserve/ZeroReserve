!include("../Common/retroshare_plugin.pri"):error("Could not include file ../Common/retroshare_plugin.pri")

# Use bogus currency. If Testnet is a compile time option of the Bitcoin code, set it there, too
DEFINES += ZR_TESTNET

isEmpty(ZR_BITCOIN){
    # One of those must be defined
    # ZR_BITCOIN = ZR_LIBBITCOIN
    # ZR_BITCOIN = ZR_DUMMYBITCOIN
    ZR_BITCOIN = ZR_SATOSHI
}


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
    RSZRRemoteItems.cpp \
    TmLocalCoordinator.cpp \
    TmLocalCohorte.cpp \
    NewWallet.cpp \
    PeerAddressDialog.cpp \
    BtcContract.cpp \
    TmContract.cpp \
    CurrentTxList.cpp

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
    RSZRRemoteItems.h \
    TmLocalCoordinator.h \
    TmLocalCohorte.h \
    ZRBitcoin.h \
    ZrLibBitcoin.h \
    NewWallet.h \
    PeerAddressDialog.h \
    BtcContract.h \
    TmContract.h \
    CurrentTxList.h

FORMS = ZeroReserveDialog.ui \
    frienddetailsdialog.ui \
    paymentdialog.ui \
    ZeroReserveConfig.ui \
    RemotePaymentDialog.ui \
    RemotePaymentRequestDialog.ui \
    NewWallet.ui \
    PeerAddressDialog.ui \
    CurrentTxList.ui

TARGET = ZeroReserve

RESOURCES = ZeroReserve_images.qrc


contains(ZR_BITCOIN, ZR_LIBBITCOIN) {
    HEADERS += ZrLibBitcoin.h
    SOURCES += ZrLibBitcoin.cpp
    QMAKE_CXXFLAGS += -std=c++03
    QMAKE_LFLAGS = -Wl,-enable-stdcall-fixup $(QMAKE_LFLAGS)
    LIBS += -lbitcoin -lleveldb -lcurl -lboost_thread -lboost_system -lboost_regex -lboost_filesystem
    DEFINES += LEVELDB_ENABLED
}

contains(ZR_BITCOIN, ZR_DUMMYBITCOIN) {
    HEADERS += ZrDummyBitcoin.h
    SOURCES += ZrDummyBitcoin.cpp
}

contains(ZR_BITCOIN, ZR_SATOSHI) {
    HEADERS += ZrSatoshiBitcoin.h
    SOURCES += ZrSatoshiBitcoin.cpp
    QMAKE_CXXFLAGS += -std=c++03
    QMAKE_LFLAGS = -Wl,-enable-stdcall-fixup $(QMAKE_LFLAGS)
    LIBS    += -L$(HOME)/lib -lnmcrpc -lcurl -ljsoncpp
    INCLUDEPATH += $(HOME)/lib
}

