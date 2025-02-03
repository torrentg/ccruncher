QT -= core gui
TARGET = ccruncher-inf
CONFIG -= qt
CONFIG += c++14 console thread
VERSION = 2.5.0

HEADERS += \
    src/utils/Exception.hpp \
    src/inference/Component.hpp \
    src/inference/Metropolis.hpp

SOURCES += \
    src/ccruncher-inf.cpp \
    src/utils/Exception.cpp \
    src/inference/Component.cpp \
    src/inference/Metropolis.cpp

INCLUDEPATH += \
    $$PWD/src

LIBS += \
    -lm \
    -lgsl \
    -lgslcblas \
    -lconfig++

CONFIG(release, debug|release) {
  DEFINES += NDEBUG
}

QMAKE_CXXFLAGS += -fopenmp -Wall -Wextra -Wshadow -Wpedantic
QMAKE_CXXFLAGS_RELEASE -= -g
QMAKE_LFLAGS *= -fopenmp

OBJECTS_DIR = $$PWD/build
DESTDIR = $$PWD/build
