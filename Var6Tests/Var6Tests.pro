QT += testlib core network sql
QT -= gui

CONFIG += c++17 console testcase
CONFIG -= app_bundle

TARGET = Var6Tests

INCLUDEPATH += ../Var6Server/src

SOURCES += \
    src/main_test.cpp \
    ../Var6Server/src/sha1.cpp \
    ../Var6Server/src/rsa.cpp \
    ../Var6Server/src/newtonsolver.cpp \
    ../Var6Server/src/audiosteganography.cpp

HEADERS += \
    src/test_sha1.h \
    src/test_rsa.h \
    src/test_newton.h \
    src/test_stego.h \
    ../Var6Server/src/sha1.h \
    ../Var6Server/src/rsa.h \
    ../Var6Server/src/newtonsolver.h \
    ../Var6Server/src/audiosteganography.h
