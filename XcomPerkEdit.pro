######################################################################
# Automatically generated by qmake (3.1) Fri Jul 5 17:34:01 2024
######################################################################

TEMPLATE = app
TARGET = XcomPerkEdit
CONFIG += qt gui c++17
INCLUDEPATH += .
QT += widgets gui
# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input

INCLUDEPATH += xcomsave/minilzo-2.09 \
               xcomsave/json11 \
               xcomsave/zlib \
               Qt 

HEADERS += MainWindow.h \
            utils.h \
            build/ui_QtDesigner.h \
            xcomsave/json11/json11.hpp \
            Qt/PerkButton.h \
            xcomsave/xcom.h \
            xcomsave/xcomio.h \
            xcomsave/util.h 

FORMS += QtDesigner_v2.ui

SOURCES += main.cpp \
            MainWindow.cpp \
            utils.cpp \
            xcomsave/json11/json11.cpp \
            Qt/PerkButton.cpp \
            xcomsave/xcom2json.cpp \
            xcomsave/json2xcom.cpp \
            xcomsave/util.cpp \
            xcomsave/xcomio.cpp \
            xcomsave/xcomreader.cpp \
            xcomsave/xcomwriter.cpp \
            xcomsave/xcomerror.cpp \
            xcomsave/minilzo-2.09/minilzo.c

# Platform-specific configurations
unix {
    QMAKE_CXXFLAGS += -Wall -Wextra -Wno-missing-field-initializers
    DEFINES += HAVE_UNISTD_H
}

win32 {
    QMAKE_CXXFLAGS += -EHsc -W3
    QMAKE_CFLAGS += -W3
    DEFINES += _CRT_SECURE_NO_WARNINGS
    DEFINES += _CRT_NONSTDC_NO_DEPRECATE
}

QMAKE_CXXFLAGS += -lstdc++fs
QMAKE_LFLAGS += -lstdc++fs

# macOS specific settings
macx {
    LIBS += -liconv
}

# Link libraries
LIBS += -lz
