QT += core gui
TARGET = ccruncher-gui
TEMPLATE = app
CONFIG += qwt

HEADERS += \
    src/gui/MainWindow.hpp \
    src/gui/DefinesDialog.hpp \
    src/gui/QStreamBuf.hpp \
    src/gui/FindDefines.hpp \
    src/gui/SimulationTask.hpp \
    src/gui/SimulationWidget.hpp \
    src/gui/AnalysisWidget.hpp \
    src/gui/AnalysisTask.hpp \
    src/gui/ProgressWidget.hpp \
    src/gui/FrameOverlay.hpp \
    src/gui/TextBrowser.hpp \
    src/gui/XmlEditWidget.hpp \
    src/gui/xmlhighlighter.hpp \
    src/params/Correlations.hpp \
    src/params/Params.hpp \
    src/params/Interest.hpp \
    src/params/Ratings.hpp \
    src/params/Sectors.hpp \
    src/params/Segmentations.hpp \
    src/params/Segmentation.hpp \
    src/params/Transitions.hpp \
    src/params/DefaultProbabilities.hpp \
    src/kernel/IData.hpp \
    src/kernel/Aggregator.hpp \
    src/kernel/MonteCarlo.hpp \
    src/kernel/SimulatedData.hpp \
    src/kernel/SimulationThread.hpp \
    src/kernel/Inverses.hpp \
    src/portfolio/Recovery.hpp \
    src/portfolio/Portfolio.hpp \
    src/portfolio/Obligor.hpp \
    src/portfolio/Exposure.hpp \
    src/portfolio/DateValues.hpp \
    src/portfolio/Asset.hpp \
    src/utils/PowMatrix.hpp \
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
    src/utils/CsvFile.hpp \
    src/utils/config.h

SOURCES += \
    src/ccruncher-gui.cpp \
    src/gui/DefinesDialog.cpp \
    src/gui/MainWindow.cpp \
    src/gui/QStreamBuf.cpp \
    src/gui/FindDefines.cpp \
    src/gui/SimulationTask.cpp \
    src/gui/SimulationWidget.cpp \
    src/gui/AnalysisWidget.cpp \
    src/gui/AnalysisTask.cpp \
    src/gui/ProgressWidget.cpp \
    src/gui/XmlEditWidget.cpp \
    src/gui/xmlhighlighter.cpp \
    src/params/Correlations.cpp \
    src/params/Interest.cpp \
    src/params/Params.cpp \
    src/params/Ratings.cpp \
    src/params/Sectors.cpp \
    src/params/Segmentations.cpp \
    src/params/Segmentation.cpp \
    src/params/Transitions.cpp \
    src/params/DefaultProbabilities.cpp \
    src/kernel/IData.cpp \
    src/kernel/Aggregator.cpp \
    src/kernel/MonteCarlo.cpp \
    src/kernel/SimulatedData.cpp \
    src/kernel/SimulationThread.cpp \
    src/kernel/Inverses.cpp \
    src/portfolio/Recovery.cpp \
    src/portfolio/Portfolio.cpp \
    src/portfolio/Obligor.cpp \
    src/portfolio/Exposure.cpp \
    src/portfolio/DateValues.cpp \
    src/portfolio/Asset.cpp \
    src/utils/PowMatrix.cpp \
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
    src/utils/CsvFile.cpp

FORMS += \
    src/gui/MainWindow.ui \
    src/gui/DefinesDialog.ui \
    src/gui/SimulationWidget.ui \
    src/gui/AnalysisWidget.ui \
    src/gui/ProgressWidget.ui \
    src/gui/XmlEditWidget.ui

INCLUDEPATH += \
    $$PWD/src

LIBS += \
    -lm \
    -lpthread

unix {
  INCLUDEPATH += \
    /usr/include/qwt
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
  RC_FILE = $$PWD/src/gui/ccruncher-gui.rc
}

CONFIG(release, debug|release) {
  DEFINES += NDEBUG
}

UI_DIR = $$PWD/build
MOC_DIR = $$PWD/build
OBJECTS_DIR = $$PWD/build
DESTDIR = $$PWD/build

OTHER_FILES += \
  src/gui/ccruncher-gui.rc
