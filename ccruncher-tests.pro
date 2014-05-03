QT -= core gui
TARGET = ccruncher-tests
CONFIG -= qt
CONFIG += c++11 console

HEADERS += \
    src/params/Correlations.hpp \
    src/params/Params.hpp \
    src/params/Interest.hpp \
    src/params/Ratings.hpp \
    src/params/Factor.hpp \
    src/params/Factors.hpp \
    src/params/Segmentations.hpp \
    src/params/Segmentation.hpp \
    src/params/Transitions.hpp \
    src/params/TransitionsTest.hpp \
    src/params/SegmentationTest.hpp \
    src/params/SegmentationsTest.hpp \
    src/params/FactorsTest.hpp \
    src/params/RatingsTest.hpp \
    src/params/ParamsTest.hpp \
    src/params/InterestTest.hpp \
    src/params/CorrelationsTest.hpp \
    src/params/DefaultProbabilities.hpp \
    src/params/DefaultProbabilitiesTest.hpp \
    src/kernel/IData.hpp \
    src/kernel/Aggregator.hpp \
    src/kernel/MonteCarlo.hpp \
    src/kernel/SimulatedData.hpp \
    src/kernel/SimulationThread.hpp \
    src/kernel/SimulatedDataTest.hpp \
    src/kernel/IDataTest.hpp \
    src/kernel/Inverses.hpp \
    src/kernel/InversesTest.hpp \
    src/portfolio/LGD.hpp \
    src/portfolio/Portfolio.hpp \
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
    src/utils/Timer.hpp \
    src/utils/Thread.hpp \
    src/utils/Parser.hpp \
    src/utils/Logger.hpp \
    src/utils/Format.hpp \
    src/utils/File.hpp \
    src/utils/ExpatUserData.hpp \
    src/utils/ExpatParser.hpp \
    src/utils/ExpatHandlers.hpp \
    src/utils/Exception.hpp \
    src/utils/Date.hpp \
    src/utils/Expr.hpp \
    src/utils/config.h \
    src/utils/UtilsTest.hpp \
    src/utils/TimerTest.hpp \
    src/utils/ParserTest.hpp \
    src/utils/FormatTest.hpp \
    src/utils/FileTest.hpp \
    src/utils/ExpatUserDataTest.hpp \
    src/utils/ExceptionTest.hpp \
    src/utils/DateTest.hpp \
    src/utils/ExprTest.hpp \
    src/utils/MiniCppUnit.hxx

SOURCES += \
    src/ccruncher-tests.cpp \
    src/params/Correlations.cpp \
    src/params/Interest.cpp \
    src/params/Params.cpp \
    src/params/Ratings.cpp \
    src/params/Factor.cpp \
    src/params/Factors.cpp \
    src/params/Segmentations.cpp \
    src/params/Segmentation.cpp \
    src/params/Transitions.cpp \
    src/params/TransitionsTest.cpp \
    src/params/SegmentationTest.cpp \
    src/params/SegmentationsTest.cpp \
    src/params/FactorsTest.cpp \
    src/params/RatingsTest.cpp \
    src/params/ParamsTest.cpp \
    src/params/InterestTest.cpp \
    src/params/CorrelationsTest.cpp \
    src/params/DefaultProbabilities.cpp \
    src/params/DefaultProbabilitiesTest.cpp \
    src/kernel/IData.cpp \
    src/kernel/Aggregator.cpp \
    src/kernel/MonteCarlo.cpp \
    src/kernel/SimulatedData.cpp \
    src/kernel/SimulationThread.cpp \
    src/kernel/SimulatedDataTest.cpp \
    src/kernel/IDataTest.cpp \
    src/kernel/Inverses.cpp \
    src/kernel/InversesTest.cpp \
    src/portfolio/LGD.cpp \
    src/portfolio/Portfolio.cpp \
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
    src/utils/Timer.cpp \
    src/utils/Thread.cpp \
    src/utils/Parser.cpp \
    src/utils/Logger.cpp \
    src/utils/Format.cpp \
    src/utils/File.cpp \
    src/utils/ExpatUserData.cpp \
    src/utils/ExpatParser.cpp \
    src/utils/ExpatHandlers.cpp \
    src/utils/Exception.cpp \
    src/utils/Date.cpp \
    src/utils/Expr.cpp \
    src/utils/UtilsTest.cpp \
    src/utils/TimerTest.cpp \
    src/utils/ParserTest.cpp \
    src/utils/FormatTest.cpp \
    src/utils/FileTest.cpp \
    src/utils/ExpatUserDataTest.cpp \
    src/utils/ExceptionTest.cpp \
    src/utils/DateTest.cpp \
    src/utils/ExprTest.cpp \
    src/utils/MiniCppUnit.cxx

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
    C:/MinGW/msys/1.0/local/include \
    C:/MinGW/include
  LIBS += \
    C:/MinGW/lib/libz.a \
    C:/MinGW/msys/1.0/local/lib/libexpat.a \
    C:/MinGW/msys/1.0/local/lib/libgsl.a \
    C:/MinGW/msys/1.0/local/lib/libgslcblas.a
}

CONFIG(release, debug|release) {
  DEFINES += NDEBUG
}

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -Wshadow -Wpedantic

OBJECTS_DIR = $$PWD/build
DESTDIR = $$PWD/build

