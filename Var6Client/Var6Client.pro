QT += core gui widgets network

CONFIG += c++17
CONFIG -= app_bundle

TARGET = Var6Client

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/loginwindow.cpp \
    src/networkmanager.cpp \
    src/functionpanel.cpp \
    src/resulttable.cpp \
    src/adminpanel.cpp

HEADERS += \
    src/mainwindow.h \
    src/loginwindow.h \
    src/networkmanager.h \
    src/functionpanel.h \
    src/resulttable.h \
    src/adminpanel.h
