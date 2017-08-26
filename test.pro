#-------------------------------------------------
#
# Project created by QtCreator 2017-05-08T21:09:40
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    ftdi.cpp \
    global.cpp \
    widget.cpp \
    signalprocessing.cpp \
    datareciever.cpp

HEADERS  += mainwindow.h \
    ftdi.h \
    global.h \
    lowpassfilterparam.h \
    tmwtypes.h \
    widget.h \
    dll2xx/ftd2xx.h \
    signalprocessing.h \
    datareciever.h \
    signalprocessing.h

FORMS    += mainwindow.ui

win32: RC_ICONS += rc/Stats2.ico

win32: LIBS += -L$$PWD/dll2xx/ -lftd2xx

INCLUDEPATH += $$PWD/dll2xx
DEPENDPATH += $$PWD/dll2xx

RESOURCES += \
    myrc.qrc

#OTHER_FILES += \
#    myapp.rc

#RC_FILE = myapp.rc
