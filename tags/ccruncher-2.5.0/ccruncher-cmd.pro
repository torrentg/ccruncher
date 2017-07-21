QT -= core gui
TARGET = ccruncher-cmd
CONFIG -= qt
CONFIG += c++14 console thread
VERSION = 2.5.0

HEADERS += \
    src/params/Params.hpp \
    src/params/Interest.hpp \
    src/params/Rating.hpp \
    src/params/Factor.hpp \
    src/params/Segmentation.hpp \
    src/params/Transitions.hpp \
    src/params/CDF.hpp \
    src/kernel/Aggregator.hpp \
    src/kernel/MonteCarlo.hpp \
    src/kernel/SimulationThread.hpp \
    src/kernel/Inverse.hpp \
    src/kernel/Input.hpp \
    src/kernel/InputData.hpp \
    src/kernel/XmlInputData.hpp \
    src/portfolio/LGD.hpp \
    src/portfolio/Obligor.hpp \
    src/portfolio/EAD.hpp \
    src/portfolio/DateValues.hpp \
    src/portfolio/Asset.hpp \
    src/utils/PowMatrix.hpp \
    src/utils/Utils.hpp \
    src/utils/Thread.hpp \
    src/utils/Parser.hpp \
    src/utils/Logger.hpp \
    src/utils/MacrosBuffer.hpp \
    src/utils/ExpatParser.hpp \
    src/utils/ExpatHandlers.hpp \
    src/utils/Exception.hpp \
    src/utils/Date.hpp \
    src/utils/Expr.hpp \
    src/utils/config.h

SOURCES += \
    src/ccruncher-cmd.cpp \
    src/params/Params.cpp \
    src/params/Interest.cpp \
    src/params/Rating.cpp \
    src/params/Factor.cpp \
    src/params/Segmentation.cpp \
    src/params/Transitions.cpp \
    src/params/CDF.cpp \
    src/kernel/Aggregator.cpp \
    src/kernel/MonteCarlo.cpp \
    src/kernel/SimulationThread.cpp \
    src/kernel/Inverse.cpp \
    src/kernel/Input.cpp \
    src/kernel/InputData.cpp \
    src/kernel/XmlInputData.cpp \
    src/portfolio/LGD.cpp \
    src/portfolio/Obligor.cpp \
    src/portfolio/EAD.cpp \
    src/portfolio/DateValues.cpp \
    src/portfolio/Asset.cpp \
    src/utils/PowMatrix.cpp \
    src/utils/Utils.cpp \
    src/utils/Thread.cpp \
    src/utils/Parser.cpp \
    src/utils/Logger.cpp \
    src/utils/MacrosBuffer.cpp \
    src/utils/ExpatParser.cpp \
    src/utils/ExpatHandlers.cpp \
    src/utils/Exception.cpp \
    src/utils/Date.cpp \
    src/utils/Expr.cpp

INCLUDEPATH += \
    $$PWD/src

LIBS += \
    -lm 

unix {
  LIBS += \
    -lz \
    -lgsl \
    -lgslcblas \
    -lexpat
}

win32 {
  INCLUDEPATH += \
    C:/MinGW/msys/1.0/local/include
  LIBS += \
    C:/MinGW/lib/libz.a \
    C:/MinGW/msys/1.0/local/lib/libexpat.a \
    C:/MinGW/msys/1.0/local/lib/libgsl.a \
    C:/MinGW/msys/1.0/local/lib/libgslcblas.a
}

CONFIG(release, debug|release) {
  DEFINES += NDEBUG
}

QMAKE_CXXFLAGS += -Wall -Wextra -Wshadow -Wpedantic
QMAKE_CXXFLAGS_RELEASE -= -g

OBJECTS_DIR = $$PWD/build
DESTDIR = $$PWD/build

