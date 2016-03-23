#-------------------------------------------------
#
# Project created by QtCreator 2016-02-19T12:38:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = traffic_simulator_qt
TEMPLATE = app


SOURCES += traffic_simulator.cpp\
        test.cpp\
        bus.cpp\
        passenger.cpp\
		loop.cpp\
    busstop.cpp

HEADERS  += traffic_simulator.h\
         header.h\
         bus.h\
         passenger.h\
		 loop.h\
		 graph.h\
    busstop.h

FORMS    += traffic_simulator.ui

CONFIG   += c++11
