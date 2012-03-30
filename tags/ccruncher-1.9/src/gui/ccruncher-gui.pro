#-------------------------------------------------
#
# Project created by QtCreator 2011-07-05T08:05:23
#
#-------------------------------------------------

QT       += core gui sql
TARGET = ccruncher-gui
TEMPLATE = app

SOURCES += main.cpp\
	MainWindow.cpp \
    Database.cpp \
    ConfigFile.cpp \
    Configuration.cpp \
    ../correlations/CorrelationMatrix.cpp \
    ../interests/Interest.cpp \
    ../kernel/SimulationThread.cpp \
    ../kernel/SimulatedData.cpp \
    ../kernel/MonteCarlo.cpp \
    ../kernel/IData.cpp \
    ../kernel/Aggregator.cpp \
    ../math/PowMatrix.cpp \
    ../math/BlockTStudentCopula.cpp \
    ../math/BlockMatrixChol.cpp \
    ../math/BlockGaussianCopula.cpp \
    ../math/FastTStudentCdf.cpp \
    ../params/Params.cpp \
    ../portfolio/Recovery.cpp \
    ../portfolio/Portfolio.cpp \
    ../portfolio/Obligor.cpp \
    ../portfolio/Exposure.cpp \
    ../portfolio/DateValues.cpp \
    ../portfolio/Asset.cpp \
    ../ratings/Ratings.cpp \
    ../ratings/Rating.cpp \
    ../sectors/Sectors.cpp \
    ../sectors/Sector.cpp \
    ../segmentations/Segmentations.cpp \
    ../segmentations/Segmentation.cpp \
    ../survival/Survival.cpp \
    ../transitions/TransitionMatrix.cpp \
    ../utils/Utils.cpp \
    ../utils/Timer.cpp \
    ../utils/Thread.cpp \
    ../utils/Strings.cpp \
    ../utils/Parser.cpp \
    ../utils/Logger.cpp \
    ../utils/Format.cpp \
    ../utils/File.cpp \
    ../utils/ExpatUserData.cpp \
    ../utils/ExpatParser.cpp \
    ../utils/ExpatHandlers.cpp \
    ../utils/Exception.cpp \
    ../utils/Date.cpp \
    ../utils/Arrays.cpp \
    ../../deps/gzstream-1.5/gzstream.cpp

win32 {
SOURCES += \
	../win32/gettimeofday.c \
	../win32/getopt.c \
	../win32/dirent.c \
}

HEADERS  += MainWindow.hpp \
    Database.hpp \
    ConfigFile.hpp \
    Configuration.hpp \
    ../correlations/CorrelationMatrix.hpp \
    ../interests/Interest.hpp \
    ../kernel/SimulationThread.hpp \
    ../kernel/SimulatedData.hpp \
    ../kernel/MonteCarlo.hpp \
    ../kernel/IData.hpp \
    ../kernel/Aggregator.hpp \
    ../math/PowMatrix.hpp \
    ../math/Copula.hpp \
    ../math/BlockTStudentCopula.hpp \
    ../math/BlockMatrixChol.hpp \
    ../math/BlockGaussianCopula.hpp \
    ../math/FastTStudentCdf.hpp \
    ../params/Params.hpp \
    ../portfolio/Recovery.hpp \
    ../portfolio/Portfolio.hpp \
    ../portfolio/Obligor.hpp \
    ../portfolio/Exposure.hpp \
    ../portfolio/DateValues.hpp \
    ../portfolio/Asset.hpp \
    ../ratings/Ratings.hpp \
    ../ratings/Rating.hpp \
    ../sectors/Sectors.hpp \
    ../sectors/Sector.hpp \
    ../segmentations/Segmentations.hpp \
    ../segmentations/Segmentation.hpp \
    ../survival/Survival.hpp \
    ../transitions/TransitionMatrix.hpp \
    ../utils/Utils.hpp \
    ../utils/Timer.hpp \
    ../utils/Thread.hpp \
    ../utils/Strings.hpp \
    ../utils/Parser.hpp \
    ../utils/Logger.hpp \
    ../utils/Format.hpp \
    ../utils/File.hpp \
    ../utils/ExpatUserData.hpp \
    ../utils/ExpatParser.hpp \
    ../utils/ExpatHandlers.hpp \
    ../utils/Exception.hpp \
    ../utils/Date.hpp \
    ../utils/config.h \
	../utils/Arrays.hpp \
	../../deps/gzstream-1.5/gzstream.h

win32 {
HEADERS += \
	../win32/winconfig.h \
	../win32/gettimeofday.h \
	../win32/getopt.h \
	../win32/dirent.h
}

INCLUDEPATH += ../ \
	../../deps/gzstream-1.5/

LIBS += \
	-lm \
	-lexpat \
	-lz \
	-lgslcblas \
	-lgsl \
	-lpthread

FORMS    += MainWindow.ui

OTHER_FILES += \
    ccruncher.conf
