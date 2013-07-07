!include("../Common/retroshare_plugin.pri"): error("Could not include file ../Common/retroshare_plugin.pri")


CONFIG += qt uic qrc resources

SOURCES = ZeroReserveDialog.cpp ZeroReservePlugin.cpp
HEADERS = ZeroReserveDialog.h ZeroReservePlugin.h
FORMS   = ZeroReserveDialog.ui

TARGET = ZeroReserve

RESOURCES = ZeroReserve_images.qrc 

