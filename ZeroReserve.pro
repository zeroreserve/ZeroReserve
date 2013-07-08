!include("../Common/retroshare_plugin.pri"): error("Could not include file ../Common/retroshare_plugin.pri")


CONFIG += qt uic qrc resources

SOURCES = ZeroReserveDialog.cpp ZeroReservePlugin.cpp \
    frienddetailsdialog.cpp \
    paymentdialog.cpp
HEADERS = ZeroReserveDialog.h ZeroReservePlugin.h \
    frienddetailsdialog.h \
    paymentdialog.h
FORMS   = ZeroReserveDialog.ui \
    frienddetailsdialog.ui \
    paymentdialog.ui

TARGET = ZeroReserve

RESOURCES = ZeroReserve_images.qrc 
