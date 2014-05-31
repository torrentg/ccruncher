# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = \
    ../src\
    ../ccruncher-cmd.pro\
    ../ccruncher-gui.pro\
    ../ccruncher-tests.pro\
    ../Doxyfile

noinst_LIBRARIES = libsim.a

bin_PROGRAMS = ccruncher-cmd

check_PROGRAMS = ccruncher-tests

TESTS = ccruncher-tests

# -------------------------------------------------------------
# per-object specific compilation options
# http://www.gnu.org/software/automake/manual/html_node/Per_002dObject-Flags.html
# -------------------------------------------------------------

libsim_a_SOURCES = \
    ../src/kernel/SimulationThread.cpp\
    ../src/kernel/SimulationThread.hpp

libsim_a_CXXFLAGS = 
# -ftree-vectorize -msse2 -ffast-math -ftree-vectorizer-verbose=2

# -------------------------------------------------------------
# ccruncher-cmd
# -------------------------------------------------------------
ccruncher_cmd_SOURCES = \
    ../src/ccruncher-cmd.cpp\
    ../src/kernel/MonteCarlo.cpp\
    ../src/kernel/SimulatedData.cpp\
    ../src/kernel/IData.cpp\
    ../src/kernel/Aggregator.cpp\
    ../src/kernel/Inverses.cpp\
    ../src/params/Params.cpp\
    ../src/params/Interest.cpp\
    ../src/params/Rating.cpp\
    ../src/params/Ratings.cpp\
    ../src/params/Transitions.cpp\
    ../src/params/Factor.cpp\
    ../src/params/Factors.cpp\
    ../src/params/Correlations.cpp\
    ../src/params/Segmentations.cpp\
    ../src/params/Segmentation.cpp\
    ../src/params/CDF.cpp\
    ../src/params/DefaultProbabilities.cpp\
    ../src/portfolio/Portfolio.cpp\
    ../src/portfolio/Obligor.cpp\
    ../src/portfolio/Asset.cpp\
    ../src/portfolio/DateValues.cpp\
    ../src/portfolio/LGD.cpp\
    ../src/portfolio/EAD.cpp\
    ../src/utils/PowMatrix.cpp\
    ../src/utils/Exception.cpp\
    ../src/utils/Logger.cpp\
    ../src/utils/Parser.cpp\
    ../src/utils/File.cpp\
    ../src/utils/Date.cpp\
    ../src/utils/Expr.cpp\
    ../src/utils/Timer.cpp\
    ../src/utils/ExpatParser.cpp\
    ../src/utils/ExpatUserData.cpp\
    ../src/utils/ExpatHandlers.cpp\
    ../src/utils/Utils.cpp\
    ../src/utils/Format.cpp\
    ../src/utils/Thread.cpp\
    \
    ../src/kernel/MonteCarlo.hpp\
    ../src/kernel/SimulatedData.hpp\
    ../src/kernel/IData.hpp\
    ../src/kernel/Aggregator.hpp\
    ../src/kernel/Inverses.hpp\
    ../src/params/Params.hpp\
    ../src/params/Interest.hpp\
    ../src/params/Rating.hpp\
    ../src/params/Ratings.hpp\
    ../src/params/Transitions.hpp\
    ../src/params/Factor.hpp\
    ../src/params/Factors.hpp\
    ../src/params/Correlations.hpp\
    ../src/params/Segmentations.hpp\
    ../src/params/Segmentation.hpp\
    ../src/params/CDF.hpp\
    ../src/params/DefaultProbabilities.hpp\
    ../src/portfolio/Portfolio.hpp\
    ../src/portfolio/Obligor.hpp\
    ../src/portfolio/Asset.hpp\
    ../src/portfolio/DateValues.hpp\
    ../src/portfolio/LGD.hpp\
    ../src/portfolio/EAD.hpp\
    ../src/utils/PowMatrix.hpp\
    ../src/utils/Exception.hpp\
    ../src/utils/Logger.hpp\
    ../src/utils/Parser.hpp\
    ../src/utils/File.hpp\
    ../src/utils/Date.hpp\
    ../src/utils/Expr.hpp\
    ../src/utils/Timer.hpp\
    ../src/utils/ExpatParser.hpp\
    ../src/utils/ExpatUserData.hpp\
    ../src/utils/ExpatHandlers.hpp\
    ../src/utils/Utils.hpp\
    ../src/utils/Format.hpp\
    ../src/utils/Thread.hpp\
    ../src/utils/config.h

ccruncher_cmd_CXXFLAGS =
ccruncher_cmd_LDADD = libsim.a
ccruncher_cmd_LDFLAGS =

# -------------------------------------------------------------
# ccruncher-tests
# -------------------------------------------------------------
ccruncher_tests_SOURCES = \
    ../src/ccruncher-tests.cpp\
    ../src/utils/MiniCppUnit.cxx\
    ../src/utils/TimerTest.cpp\
    ../src/utils/DateTest.cpp\
    ../src/utils/ExprTest.cpp\
    ../src/utils/ParserTest.cpp\
    ../src/utils/ExceptionTest.cpp\
    ../src/utils/FileTest.cpp\
    ../src/utils/FormatTest.cpp\
    ../src/utils/UtilsTest.cpp\
    ../src/utils/ExpatUserDataTest.cpp\
    ../src/utils/PowMatrixTest.cpp\
    ../src/portfolio/AssetTest.cpp\
    ../src/portfolio/DateValuesTest.cpp\
    ../src/portfolio/LGDTest.cpp\
    ../src/portfolio/EADTest.cpp\
    ../src/portfolio/ObligorTest.cpp\
    ../src/params/InterestTest.cpp\
    ../src/params/SegmentationTest.cpp\
    ../src/params/SegmentationsTest.cpp\
    ../src/params/ParamsTest.cpp\
    ../src/params/CorrelationsTest.cpp\
    ../src/params/RatingsTest.cpp\
    ../src/params/FactorsTest.cpp\
    ../src/params/TransitionsTest.cpp\
    ../src/params/CDFTest.cpp\
    ../src/params/DefaultProbabilitiesTest.cpp\
    ../src/kernel/IDataTest.cpp\
    ../src/kernel/SimulatedDataTest.cpp\
    ../src/kernel/InversesTest.cpp\
    \
    ../src/utils/Timer.cpp\
    ../src/utils/Parser.cpp\
    ../src/utils/Logger.cpp\
    ../src/utils/Date.cpp\
    ../src/utils/Expr.cpp\
    ../src/utils/Exception.cpp\
    ../src/utils/File.cpp\
    ../src/utils/ExpatParser.cpp\
    ../src/utils/ExpatUserData.cpp\
    ../src/utils/ExpatHandlers.cpp\
    ../src/utils/Utils.cpp\
    ../src/utils/Format.cpp\
    ../src/utils/Thread.cpp\
    ../src/utils/PowMatrix.cpp\
    ../src/portfolio/Portfolio.cpp\
    ../src/portfolio/Obligor.cpp\
    ../src/portfolio/Asset.cpp\
    ../src/portfolio/DateValues.cpp\
    ../src/portfolio/LGD.cpp\
    ../src/portfolio/EAD.cpp\
    ../src/params/Factor.cpp\
    ../src/params/Factors.cpp\
    ../src/params/Rating.cpp\
    ../src/params/Ratings.cpp\
    ../src/params/Params.cpp\
    ../src/params/Transitions.cpp\
    ../src/params/Correlations.cpp\
    ../src/params/Interest.cpp\
    ../src/params/Segmentations.cpp\
    ../src/params/Segmentation.cpp\
    ../src/params/CDF.cpp\
    ../src/params/DefaultProbabilities.cpp\
    ../src/kernel/MonteCarlo.cpp\
    ../src/kernel/SimulatedData.cpp\
    ../src/kernel/IData.cpp\
    ../src/kernel/Aggregator.cpp\
    ../src/kernel/Inverses.cpp\
    \
    ../src/utils/MiniCppUnit.hxx\
    ../src/utils/TimerTest.hpp\
    ../src/utils/DateTest.hpp\
    ../src/utils/ExprTest.hpp\
    ../src/utils/ParserTest.hpp\
    ../src/utils/ExceptionTest.hpp\
    ../src/utils/FileTest.hpp\
    ../src/utils/FormatTest.hpp\
    ../src/utils/UtilsTest.hpp\
    ../src/utils/ExpatUserDataTest.hpp\
    ../src/utils/PowMatrixTest.hpp\
    ../src/portfolio/AssetTest.hpp\
    ../src/portfolio/DateValuesTest.hpp\
    ../src/portfolio/LGDTest.hpp\
    ../src/portfolio/EADTest.hpp\
    ../src/portfolio/ObligorTest.hpp\
    ../src/params/CorrelationsTest.hpp\
    ../src/params/InterestTest.hpp\
    ../src/params/SegmentationTest.hpp\
    ../src/params/SegmentationsTest.hpp\
    ../src/params/ParamsTest.hpp\
    ../src/params/RatingsTest.hpp\
    ../src/params/FactorsTest.hpp\
    ../src/params/TransitionsTest.hpp\
    ../src/params/CDFTest.hpp\
    ../src/params/DefaultProbabilitiesTest.hpp\
    ../src/kernel/IDataTest.hpp\
    ../src/kernel/SimulatedDataTest.hpp\
    ../src/kernel/InversesTest.hpp\
    \
    ../src/utils/Timer.hpp\
    ../src/utils/Parser.hpp\
    ../src/utils/Logger.hpp\
    ../src/utils/Date.hpp\
    ../src/utils/Expr.hpp\
    ../src/utils/Exception.hpp\
    ../src/utils/File.hpp\
    ../src/utils/ExpatParser.hpp\
    ../src/utils/ExpatUserData.hpp\
    ../src/utils/ExpatHandlers.hpp\
    ../src/utils/Utils.hpp\
    ../src/utils/Format.hpp\
    ../src/utils/Thread.hpp\
    ../src/utils/PowMatrix.hpp\
    ../src/portfolio/Asset.hpp\
    ../src/portfolio/DateValues.hpp\
    ../src/portfolio/LGD.hpp\
    ../src/portfolio/EAD.hpp\
    ../src/portfolio/Portfolio.hpp\
    ../src/portfolio/Obligor.hpp\
    ../src/params/Factor.hpp\
    ../src/params/Factors.hpp\
    ../src/params/Rating.hpp\
    ../src/params/Ratings.hpp\
    ../src/params/Params.hpp\
    ../src/params/Transitions.hpp\
    ../src/params/Correlations.hpp\
    ../src/params/Interest.hpp\
    ../src/params/Segmentations.hpp\
    ../src/params/Segmentation.hpp\
    ../src/params/CDF.hpp\
    ../src/params/DefaultProbabilities.hpp\
    ../src/kernel/MonteCarlo.hpp\
    ../src/kernel/SimulatedData.hpp\
    ../src/kernel/IData.hpp\
    ../src/kernel/Aggregator.hpp\
    ../src/kernel/Inverses.hpp\
    ../src/utils/config.h

ccruncher_tests_CXXFLAGS =
ccruncher_tests_LDADD = libsim.a
ccruncher_tests_LDFLAGS =

