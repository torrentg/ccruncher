QT -= core gui
TARGET = ccruncher-tests
CONFIG -= qt
CONFIG += c++11 console
VERSION = 2.5.0

HEADERS += \
    src/params/Params.hpp \
    src/params/Interest.hpp \
    src/params/Rating.hpp \
    src/params/Factor.hpp \
    src/params/Segmentation.hpp \
    src/params/Transitions.hpp \
    src/params/CDF.hpp \
    src/params/TransitionsTest.hpp \
    src/params/SegmentationTest.hpp \
    src/params/ParamsTest.hpp \
    src/params/InterestTest.hpp \
    src/params/CDFTest.hpp \
    src/kernel/Aggregator.hpp \
    src/kernel/MonteCarlo.hpp \
    src/kernel/SimulationThread.hpp \
    src/kernel/Inverse.hpp \
    src/kernel/InverseTest.hpp \
    src/kernel/Input.hpp \
    src/kernel/InputTest.hpp \
    src/kernel/InputData.hpp \
    src/kernel/XmlInputData.hpp \
    src/kernel/XmlInputDataTest.hpp \
    src/portfolio/LGD.hpp \
    src/portfolio/Obligor.hpp \
    src/portfolio/EAD.hpp \
    src/portfolio/DateValues.hpp \
    src/portfolio/Asset.hpp \
    src/portfolio/LGDTest.hpp \
    src/portfolio/ObligorTest.hpp \
    src/portfolio/EADTest.hpp \
    src/portfolio/DateValuesTest.hpp \
    src/portfolio/AssetTest.hpp \
    src/utils/PowMatrix.hpp \
    src/utils/PowMatrixTest.hpp \
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
    src/utils/config.h \
    src/utils/UtilsTest.hpp \
    src/utils/ParserTest.hpp \
    src/utils/MacrosBufferTest.hpp \
    src/utils/ExceptionTest.hpp \
    src/utils/DateTest.hpp \
    src/utils/ExprTest.hpp \
    src/utils/MiniCppUnit.hxx

SOURCES += \
    src/ccruncher-tests.cpp \
    src/params/Interest.cpp \
    src/params/Params.cpp \
    src/params/Rating.cpp \
    src/params/Factor.cpp \
    src/params/Segmentation.cpp \
    src/params/Transitions.cpp \
    src/params/CDF.cpp \
    src/params/TransitionsTest.cpp \
    src/params/SegmentationTest.cpp \
    src/params/ParamsTest.cpp \
    src/params/InterestTest.cpp \
    src/params/CDFTest.cpp \
    src/kernel/Aggregator.cpp \
    src/kernel/MonteCarlo.cpp \
    src/kernel/SimulationThread.cpp \
    src/kernel/Inverse.cpp \
    src/kernel/InverseTest.cpp \
    src/kernel/Input.cpp \
    src/kernel/InputTest.cpp \
    src/kernel/InputData.cpp \
    src/kernel/XmlInputData.cpp \
    src/kernel/XmlInputDataTest.cpp \
    src/portfolio/LGD.cpp \
    src/portfolio/Obligor.cpp \
    src/portfolio/EAD.cpp \
    src/portfolio/DateValues.cpp \
    src/portfolio/Asset.cpp \
    src/portfolio/LGDTest.cpp \
    src/portfolio/ObligorTest.cpp \
    src/portfolio/EADTest.cpp \
    src/portfolio/DateValuesTest.cpp \
    src/portfolio/AssetTest.cpp \
    src/utils/PowMatrix.cpp \
    src/utils/PowMatrixTest.cpp \
    src/utils/Utils.cpp \
    src/utils/Thread.cpp \
    src/utils/Parser.cpp \
    src/utils/Logger.cpp \
    src/utils/MacrosBuffer.cpp \
    src/utils/ExpatParser.cpp \
    src/utils/ExpatHandlers.cpp \
    src/utils/Exception.cpp \
    src/utils/Date.cpp \
    src/utils/Expr.cpp \
    src/utils/UtilsTest.cpp \
    src/utils/ParserTest.cpp \
    src/utils/MacrosBufferTest.cpp \
    src/utils/ExceptionTest.cpp \
    src/utils/DateTest.cpp \
    src/utils/ExprTest.cpp \
    src/utils/MiniCppUnit.cxx

INCLUDEPATH += \
    $$PWD/src

LIBS += \
    -lm \
    -pthread

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

QMAKE_CXXFLAGS += -pthread -std=c++11 -Wall -Wextra -Wshadow -Wpedantic

OBJECTS_DIR = $$PWD/build
DESTDIR = $$PWD/build

