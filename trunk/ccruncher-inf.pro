QT -= core gui
TARGET = ccruncher-inf
CONFIG -= qt
CONFIG += c++11 console thread
VERSION = 2.5.0

HEADERS += \
    src/utils/Exception.hpp \
    src/inference/Configuration.hpp \
    src/inference/Component.hpp \
    src/inference/Metropolis.hpp

SOURCES += \
    src/ccruncher-inf.cpp \
    src/utils/Exception.cpp \
    src/inference/Configuration.cpp \
    src/inference/Component.cpp \
    src/inference/Metropolis.cpp

INCLUDEPATH += \
    $$PWD/src

LIBS += \
    -lm \
    -lconfig++

unix {
  LIBS += \
    -lgsl \
    -lgslcblas
}

win32 {
  INCLUDEPATH += \
    C:/MinGW/msys/1.0/local/include
  LIBS += \
    C:/MinGW/msys/1.0/local/lib/libgsl.a \
    C:/MinGW/msys/1.0/local/lib/libgslcblas.a
}

CONFIG(release, debug|release) {
  DEFINES += NDEBUG
}

QMAKE_CXXFLAGS += -fopenmp -Os -std=c++11 -Wall -Wextra -Wshadow -Wpedantic
QMAKE_CXXFLAGS_RELEASE -= -g
QMAKE_LFLAGS *= -fopenmp -Os

OBJECTS_DIR = $$PWD/build
DESTDIR = $$PWD/build
