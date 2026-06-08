QT += core network sql
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = Var6Server

SOURCES += \
    src/main.cpp \
    src/server.cpp \
    src/clienthandler.cpp \
    src/database.cpp \
    src/rsa.cpp \
    src/sha1.cpp \
    src/newtonsolver.cpp \
    src/audiosteganography.cpp

HEADERS += \
    src/server.h \
    src/clienthandler.h \
    src/database.h \
    src/user.h \
    src/rsa.h \
    src/sha1.h \
    src/newtonsolver.h \
    src/audiosteganography.h
