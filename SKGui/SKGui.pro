# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = SKGui
DESTDIR = /home/uk/lib
QT += core gui
CONFIG += debug
DEFINES += QT_DLL SKGUI_LIB
INCLUDEPATH += ./GeneratedFiles \
    . \
    ./GeneratedFiles/Debug \
    ./../include/SKBase \
    ./../include/SKBaseWidget \
    ./../include/SKGui \
    /usr/include/mysql \
    /home/uk/include \
    /home/uk/include/comm \
    /home/uk/include/db \
    /home/uk/include/db/mdb \
    /home/uk/include/db/mysql \
    /home/uk/include/db/oracle \
    /home/uk/include/db/oracle/oci/include

LIBS += -L"/home/uk/lib" \
    -lSKBased \
    -lSKBaseWidget

PRECOMPILED_HEADER = StdAfx.h
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(SKGui.pri)
