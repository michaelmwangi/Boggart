TEMPLATE = app
CONFIG += static
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    broker.cpp \
    service.cpp \
    spdlog/fmt/bundled/format.cc \
    spdlog/fmt/bundled/ostream.cc

unix:!macx: LIBS += -L$$PWD/../../../usr/local/lib/ -lzmq

INCLUDEPATH += $$PWD/../../../usr/local/include
DEPENDPATH += $$PWD/../../../usr/local/include

HEADERS += \
    broker.h \
    service.h \
    blockingqueue.h \
    opcodes.h \
    spdlog/details/async_log_helper.h \
    spdlog/details/async_logger_impl.h \
    spdlog/details/file_helper.h \
    spdlog/details/log_msg.h \
    spdlog/details/logger_impl.h \
    spdlog/details/mpmc_bounded_q.h \
    spdlog/details/null_mutex.h \
    spdlog/details/os.h \
    spdlog/details/pattern_formatter_impl.h \
    spdlog/details/registry.h \
    spdlog/details/spdlog_impl.h \
    spdlog/fmt/bundled/format.h \
    spdlog/fmt/bundled/ostream.h \
    spdlog/fmt/bundled/printf.h \
    spdlog/fmt/fmt.h \
    spdlog/fmt/ostr.h \
    spdlog/sinks/android_sink.h \
    spdlog/sinks/ansicolor_sink.h \
    spdlog/sinks/base_sink.h \
    spdlog/sinks/dist_sink.h \
    spdlog/sinks/file_sinks.h \
    spdlog/sinks/msvc_sink.h \
    spdlog/sinks/null_sink.h \
    spdlog/sinks/ostream_sink.h \
    spdlog/sinks/sink.h \
    spdlog/sinks/stdout_sinks.h \
    spdlog/sinks/syslog_sink.h \
    spdlog/sinks/wincolor_sink.h \
    spdlog/async_logger.h \
    spdlog/common.h \
    spdlog/formatter.h \
    spdlog/logger.h \
    spdlog/spdlog.h \
    spdlog/tweakme.h

unix:!macx: LIBS += -L$$PWD/../../../usr/local/lib/ -lzmqpp

INCLUDEPATH += $$PWD/../../../usr/local/include
DEPENDPATH += $$PWD/../../../usr/local/include

unix:!macx: LIBS += -lpthread-2.22
unix:!macx: LIBS += -luuid
