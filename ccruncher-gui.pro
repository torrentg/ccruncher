QT += core gui network
TARGET = ccruncher-gui
TEMPLATE = app
CONFIG += c++11 qwt

HEADERS += \
    src/gui/Application.hpp \
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
    src/gui/MdiChildWidget.hpp \
    src/gui/CodeEditor.hpp \
    src/gui/QwtPlotCanvasExt.hpp \
    src/gui/QwtPieChart.hpp \
    src/params/Correlations.hpp \
    src/params/Params.hpp \
    src/params/Interest.hpp \
    src/params/Rating.hpp \
    src/params/Ratings.hpp \
    src/params/Factor.hpp \
    src/params/Factors.hpp \
    src/params/Segmentations.hpp \
    src/params/Segmentation.hpp \
    src/params/Transitions.hpp \
    src/params/CDF.hpp \
    src/params/DefaultProbabilities.hpp \
    src/kernel/IData.hpp \
    src/kernel/Aggregator.hpp \
    src/kernel/MonteCarlo.hpp \
    src/kernel/SimulationThread.hpp \
    src/kernel/Inverse.hpp \
    src/portfolio/LGD.hpp \
    src/portfolio/Portfolio.hpp \
    src/portfolio/Obligor.hpp \
    src/portfolio/EAD.hpp \
    src/portfolio/DateValues.hpp \
    src/portfolio/Asset.hpp \
    src/utils/PowMatrix.hpp \
    src/utils/Utils.hpp \
    src/utils/Timer.hpp \
    src/utils/Thread.hpp \
    src/utils/Parser.hpp \
    src/utils/Logger.hpp \
    src/utils/File.hpp \
    src/utils/ExpatUserData.hpp \
    src/utils/ExpatParser.hpp \
    src/utils/ExpatHandlers.hpp \
    src/utils/Exception.hpp \
    src/utils/Date.hpp \
    src/utils/Expr.hpp \
    src/utils/CsvFile.hpp \
    src/utils/config.h

SOURCES += \
    src/ccruncher-gui.cpp \
    src/gui/Application.cpp \
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
    src/gui/CodeEditor.cpp \
    src/gui/QwtPieChart.cpp \
    src/params/Correlations.cpp \
    src/params/Interest.cpp \
    src/params/Params.cpp \
    src/params/Rating.cpp \
    src/params/Ratings.cpp \
    src/params/Factor.cpp \
    src/params/Factors.cpp \
    src/params/Segmentations.cpp \
    src/params/Segmentation.cpp \
    src/params/Transitions.cpp \
    src/params/CDF.cpp \
    src/params/DefaultProbabilities.cpp \
    src/kernel/IData.cpp \
    src/kernel/Aggregator.cpp \
    src/kernel/MonteCarlo.cpp \
    src/kernel/SimulationThread.cpp \
    src/kernel/Inverse.cpp \
    src/portfolio/LGD.cpp \
    src/portfolio/Portfolio.cpp \
    src/portfolio/Obligor.cpp \
    src/portfolio/EAD.cpp \
    src/portfolio/DateValues.cpp \
    src/portfolio/Asset.cpp \
    src/utils/PowMatrix.cpp \
    src/utils/Utils.cpp \
    src/utils/Timer.cpp \
    src/utils/Thread.cpp \
    src/utils/Parser.cpp \
    src/utils/Logger.cpp \
    src/utils/File.cpp \
    src/utils/ExpatUserData.cpp \
    src/utils/ExpatParser.cpp \
    src/utils/ExpatHandlers.cpp \
    src/utils/Exception.cpp \
    src/utils/Date.cpp \
    src/utils/Expr.cpp \
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
    -lm

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
  RC_FILE = \
    $$PWD/src/gui/ccruncher-gui.rc
  OTHER_FILES += \
    $$PWD/src/gui/ccruncher-gui.rc
}

CONFIG(release, debug|release) {
  DEFINES += NDEBUG
}

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -Wpedantic
QMAKE_CXXFLAGS_RELEASE -= -g

UI_DIR = $$PWD/build
MOC_DIR = $$PWD/build
OBJECTS_DIR = $$PWD/build
DESTDIR = $$PWD/build
RCC_DIR = $$PWD/build

RESOURCES += \
    src/gui/ccruncher.qrc

