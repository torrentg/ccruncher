QT -= core
QT -= gui

CONFIG -= qt
CONFIG += console thread

HEADERS += \
    src/params/Correlations.hpp \
    src/params/Params.hpp \
    src/params/Interest.hpp \
    src/params/Ratings.hpp \
    src/params/Sectors.hpp \
    src/params/Segmentations.hpp \
    src/params/Segmentation.hpp \
    src/params/Survivals.hpp \
    src/params/Transitions.hpp \
    src/params/TransitionsTest.hpp \
    src/params/SurvivalsTest.hpp \
    src/params/SegmentationTest.hpp \
    src/params/SegmentationsTest.hpp \
    src/params/SectorsTest.hpp \
    src/params/RatingsTest.hpp \
    src/params/ParamsTest.hpp \
    src/params/InterestTest.hpp \
    src/params/CorrelationsTest.hpp \
    src/kernel/IData.hpp \
    src/kernel/Aggregator.hpp \
    src/kernel/MonteCarlo.hpp \
    src/kernel/SimulatedData.hpp \
    src/kernel/SimulationThread.hpp \
    src/kernel/SimulatedDataTest.hpp \
    src/kernel/IDataTest.hpp \
    src/math/PowMatrix.hpp \
    src/math/Copula.hpp \
    src/math/FastTStudentCdf.hpp \
    src/math/GMFCopula.hpp \
    src/math/TMFCopula.hpp \
    src/math/PowMatrixTest.hpp \
    src/math/FastTStudentCdfTest.hpp \
    src/math/GMFCopulaTest.hpp \
    src/math/TMFCopulaTest.hpp \
    src/portfolio/Recovery.hpp \
    src/portfolio/Portfolio.hpp \
    src/portfolio/Obligor.hpp \
    src/portfolio/Exposure.hpp \
    src/portfolio/DateValues.hpp \
    src/portfolio/Asset.hpp \
    src/portfolio/RecoveryTest.hpp \
    src/portfolio/ObligorTest.hpp \
    src/portfolio/ExposureTest.hpp \
    src/portfolio/DateValuesTest.hpp \
    src/portfolio/AssetTest.hpp \
    src/utils/Utils.hpp \
    src/utils/Timer.hpp \
    src/utils/Thread.hpp \
    src/utils/Strings.hpp \
    src/utils/Parser.hpp \
    src/utils/Logger.hpp \
    src/utils/Format.hpp \
    src/utils/File.hpp \
    src/utils/ExpatUserData.hpp \
    src/utils/ExpatParser.hpp \
    src/utils/ExpatHandlers.hpp \
    src/utils/Exception.hpp \
    src/utils/Date.hpp \
    src/utils/config.h \
    src/utils/UtilsTest.hpp \
    src/utils/TimerTest.hpp \
    src/utils/StringsTest.hpp \
    src/utils/ParserTest.hpp \
    src/utils/FormatTest.hpp \
#    src/utils/FileTest.hpp \
    src/utils/ExpatUserDataTest.hpp \
    src/utils/ExceptionTest.hpp \
    src/utils/DateTest.hpp \
    deps/gzstream-1.5/gzstream.h \
    deps/MiniCppUnit-2.5/MiniCppUnit.hxx

SOURCES += \
    src/params/Correlations.cpp \
    src/params/Interest.cpp \
    src/params/Params.cpp \
    src/params/Ratings.cpp \
    src/params/Sectors.cpp \
    src/params/Segmentations.cpp \
    src/params/Segmentation.cpp \
    src/params/Survivals.cpp \
    src/params/Transitions.cpp \
    src/params/TransitionsTest.cpp \
    src/params/SurvivalsTest.cpp \
    src/params/SegmentationTest.cpp \
    src/params/SegmentationsTest.cpp \
    src/params/SectorsTest.cpp \
    src/params/RatingsTest.cpp \
    src/params/ParamsTest.cpp \
    src/params/InterestTest.cpp \
    src/params/CorrelationsTest.cpp \
    src/kernel/IData.cpp \
    src/kernel/Aggregator.cpp \
    src/kernel/MonteCarlo.cpp \
    src/kernel/SimulatedData.cpp \
    src/kernel/SimulationThread.cpp \
    src/kernel/SimulatedDataTest.cpp \
    src/kernel/IDataTest.cpp \
    src/math/PowMatrix.cpp \
    src/math/FastTStudentCdf.cpp \
    src/math/GMFCopula.cpp \
    src/math/TMFCopula.cpp \
    src/math/PowMatrixTest.cpp \
    src/math/FastTStudentCdfTest.cpp \
    src/math/GMFCopulaTest.cpp \
    src/math/TMFCopulaTest.cpp \
    src/portfolio/Recovery.cpp \
    src/portfolio/Portfolio.cpp \
    src/portfolio/Obligor.cpp \
    src/portfolio/Exposure.cpp \
    src/portfolio/DateValues.cpp \
    src/portfolio/Asset.cpp \
    src/portfolio/RecoveryTest.cpp \
    src/portfolio/ObligorTest.cpp \
    src/portfolio/ExposureTest.cpp \
    src/portfolio/DateValuesTest.cpp \
    src/portfolio/AssetTest.cpp \
    src/utils/Utils.cpp \
    src/utils/Timer.cpp \
    src/utils/Thread.cpp \
    src/utils/Strings.cpp \
    src/utils/Parser.cpp \
    src/utils/Logger.cpp \
    src/utils/Format.cpp \
    src/utils/File.cpp \
    src/utils/ExpatUserData.cpp \
    src/utils/ExpatParser.cpp \
    src/utils/ExpatHandlers.cpp \
    src/utils/Exception.cpp \
    src/utils/Date.cpp \
    src/utils/UtilsTest.cpp \
    src/utils/TimerTest.cpp \
    src/utils/StringsTest.cpp \
    src/utils/ParserTest.cpp \
    src/utils/FormatTest.cpp \
#    src/utils/FileTest.cpp \
    src/utils/ExpatUserDataTest.cpp \
    src/utils/ExceptionTest.cpp \
    src/utils/DateTest.cpp \
    deps/gzstream-1.5/gzstream.cpp \
    deps/MiniCppUnit-2.5/TestsRunner.cxx \
    deps/MiniCppUnit-2.5/MiniCppUnit.cxx

INCLUDEPATH += \
    $$PWD/src \
    $$PWD/deps/gzstream-1.5 \
    $$PWD/deps/MiniCppUnit-2.5

LIBS += \
    -lm \
    -lpthread

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

OBJECTS_DIR = $$PWD/build
DESTDIR = $$PWD/build
