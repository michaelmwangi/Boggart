TEMPLATE = app
CONFIG += static
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    broker.cpp \
    service.cpp

unix:!macx: LIBS += -L$$PWD/../../../usr/local/lib/ -lzmq

INCLUDEPATH += $$PWD/../../../usr/local/include
DEPENDPATH += $$PWD/../../../usr/local/include

HEADERS += \
    broker.h \
    service.h \
    blockingqueue.h \
    opcodes.h

unix:!macx: LIBS += -L$$PWD/../../../usr/local/lib/ -lzmqpp

INCLUDEPATH += $$PWD/../../../usr/local/include
DEPENDPATH += $$PWD/../../../usr/local/include

unix:!macx: LIBS += -lpthread-2.22
unix:!macx: LIBS += -luuid
