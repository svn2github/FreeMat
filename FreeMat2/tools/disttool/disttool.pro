TEMPLATE = app

CONFIG -= app_bundle
CONFIG += release

TARGET = disttool

SOURCES += disttool.cpp
HEADERS += disttool.hpp

OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
DESTDIR = build
