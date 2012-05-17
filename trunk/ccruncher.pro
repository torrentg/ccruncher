HEADERS += \
    src/params/Correlations.hpp \
    src/params/Params.hpp \
    src/params/Interest.hpp \
    src/params/Ratings.hpp \
    src/params/Sectors.hpp \
    src/params/Segmentations.hpp \
    src/params/Segmentation.hpp \
    src/params/Survival.hpp \
    src/params/Transitions.hpp \
    src/kernel/IData.hpp \
    src/kernel/Aggregator.hpp \
    src/kernel/MonteCarlo.hpp \
    src/kernel/SimulatedData.hpp \
    src/kernel/SimulationThread.hpp \
    src/math/PowMatrix.hpp \
    src/math/Copula.hpp \
    src/math/BlockTStudentCopula.hpp \
    src/math/BlockMatrixChol.hpp \
    src/math/BlockMatrixCholInv.hpp \
    src/math/BlockGaussianCopula.hpp \
    src/math/FastTStudentCdf.hpp \
    src/math/CopulaCalibration.hpp \
    src/portfolio/Recovery.hpp \
    src/portfolio/Portfolio.hpp \
    src/portfolio/Obligor.hpp \
    src/portfolio/Exposure.hpp \
    src/portfolio/DateValues.hpp \
    src/portfolio/Asset.hpp \
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
    src/utils/Arrays.hpp \
    deps/gzstream-1.5/gzstream.h \
    src/params/Defaults.hpp

SOURCES += \
    src/params/Correlations.cpp \
    src/params/Interest.cpp \
    src/params/Params.cpp \
    src/params/Ratings.cpp \
    src/params/Sectors.cpp \
    src/params/Segmentations.cpp \
    src/params/Segmentation.cpp \
    src/params/Survival.cpp \
    src/params/Transitions.cpp \
    src/kernel/IData.cpp \
    src/kernel/ccruncher.cpp \
    src/kernel/Aggregator.cpp \
    src/kernel/MonteCarlo.cpp \
    src/kernel/SimulatedData.cpp \
    src/kernel/SimulationThread.cpp \
    src/math/PowMatrix.cpp \
    src/math/BlockTStudentCopula.cpp \
    src/math/BlockMatrixChol.cpp \
    src/math/BlockMatrixCholInv.cpp \
    src/math/BlockGaussianCopula.cpp \
    src/math/FastTStudentCdf.cpp \
    src/math/CopulaCalibration.cpp \
    src/portfolio/Recovery.cpp \
    src/portfolio/Portfolio.cpp \
    src/portfolio/Obligor.cpp \
    src/portfolio/Exposure.cpp \
    src/portfolio/DateValues.cpp \
    src/portfolio/Asset.cpp \
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
    src/utils/Arrays.cpp \
    deps/gzstream-1.5/gzstream.cpp \
    src/params/Defaults.cpp

INCLUDEPATH += ./src \
    deps/gzstream-1.5

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
  DEFINES += BUILD_GETOPT
  DEFINES += BUILD_DIRENT
  INCLUDEPATH += \
    deps/gsl-1.12/include \
    deps/expat-2.1.0/Source/lib \
    deps/zlib-1.2.7
  LIBS += \
    $$PWD/deps/zlib-1.2.7/libz.dll.a \
    $$PWD/deps/gsl-1.12/lib-static/libgsl.a \
    $$PWD/deps/gsl-1.12/lib-static/libgslcblas.a \
    $$PWD/deps/expat-2.1.0/Bin/libexpat.lib \
    -L"$$PWD/deps/expat-2.1.0/Bin/"
#    $$PWD/libpthreadGC2.a
}

CONFIG(release, debug|release) DEFINES += NDEBUG
