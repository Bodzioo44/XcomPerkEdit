######################################################################
# Automatically generated by qmake (3.1) Fri Jul 5 17:34:01 2024
######################################################################
TEMPLATE = app
TARGET = XcomPerkEdit
CONFIG += qt gui
INCLUDEPATH += .
QT += widgets gui
# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input

HEADERS +=  MainWindow.h \
            Soldier.h \
            build/ui_QtDesigner.h \
            Qt/PerkButton.h \
            xcomsave/xcom.h 

FORMS += QtDesigner.ui

SOURCES +=  main.cpp \
            MainWindow.cpp \
            Soldier.cpp \
            Qt/PerkButton.cpp

QMAKE_CXXFLAGS += -std=gnu++17 -Wall -Wextra -Wno-missing-field-initializers -Wno-attributes

LIBS += -L../xcomsave/build \
        -lxcomsave -lzlib
