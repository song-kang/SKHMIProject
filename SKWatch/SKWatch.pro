# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = app
TARGET = SKWatch
DESTDIR = /home/uk/bin
QT += core gui xml xmlpatterns
CONFIG += debug
DEFINES += QT_DLL QT_XML_LIB QT_XMLPATTERNS_LIB
INCLUDEPATH += ./GeneratedFiles \
    . \
    ./GeneratedFiles/Debug \
    ./../include/SKBase \
    ./../include/SKBaseWidget \
    /usr/include/mysql \
    /home/uk/include \
    /home/uk/include/comm \
    /home/uk/include/db \
    /home/uk/include/db/mdb \
    /home/uk/include/db/mysql \
    /home/uk/include/db/oracle \
    /home/uk/include/db/oracle/oci/include

LIBS += -L"/home/uk/lib" \
    -L"/home/uk/lib/instantclient_11_2" \
    -L"/usr/lib/i386-linux-gnu" \
    -lsbased \
    -lsbase_mdbd \
    -lsbase_mysqld \
    -lsbase_oracled \
    -lSKBased \
    -lSKBaseWidget \
    -lmysqlclient \
    -lociei

PRECOMPILED_HEADER = StdAfx.h
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(SKWatch.pri)
win32:RC_FILE = SKWatch.rc
