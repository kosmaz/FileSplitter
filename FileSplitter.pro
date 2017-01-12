#-------------------------------------------------
#
# Project created by QtCreator 2016-03-05T12:58:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileSplitter
TEMPLATE = app

CONFIG += c++11

RESOURCES += \
    Resources.qrc

HEADERS += \
    src/FileSplitter.hpp \
    src/Process.hpp \
    src/SegmentHeader.hpp \
    src/Widget.hpp

SOURCES += \
    src/FileSplitter.cpp \
    src/main.cpp \
    src/Process.cpp \
    src/SegmentHeader.cpp \
    src/Widget.cpp
