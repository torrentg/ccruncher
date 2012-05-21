# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = \
    ../deps/MiniCppUnit-2.5\
    ../deps/gzstream-1.5\
    ../src/win32

bin_PROGRAMS = ccruncher

check_PROGRAMS = tests samplegen

TESTS = tests 

# -------------------------------------------------------------
# ccruncher
# -------------------------------------------------------------
ccruncher_SOURCES = \
    ../deps/gzstream-1.5/gzstream.cpp\
    ../src/kernel/ccruncher.cpp\
    ../src/kernel/MonteCarlo.cpp\
    ../src/kernel/SimulationThread.cpp\
    ../src/kernel/SimulatedData.cpp\
    ../src/kernel/IData.cpp\
    ../src/kernel/Aggregator.cpp\
    ../src/params/Params.cpp\
    ../src/params/Interest.cpp\
    ../src/params/Ratings.cpp\
    ../src/params/Transitions.cpp\
    ../src/params/Survivals.cpp\
    ../src/params/Sectors.cpp\
    ../src/params/Correlations.cpp\
    ../src/params/Segmentations.cpp\
    ../src/params/Segmentation.cpp\
    ../src/params/Defaults.cpp\
    ../src/math/PowMatrix.cpp\
    ../src/math/CopulaCalibration.cpp\
    ../src/math/BlockMatrixChol.cpp\
    ../src/math/BlockMatrixCholInv.cpp\
    ../src/math/BlockGaussianCopula.cpp\
    ../src/math/BlockTStudentCopula.cpp\
    ../src/math/FastTStudentCdf.cpp\
    ../src/portfolio/Portfolio.cpp\
    ../src/portfolio/Obligor.cpp\
    ../src/portfolio/Asset.cpp\
    ../src/portfolio/DateValues.cpp\
    ../src/portfolio/Recovery.cpp\
    ../src/portfolio/Exposure.cpp\
    ../src/utils/Exception.cpp\
    ../src/utils/Logger.cpp\
    ../src/utils/Parser.cpp\
    ../src/utils/File.cpp\
    ../src/utils/Date.cpp\
    ../src/utils/Timer.cpp\
    ../src/utils/Arrays.cpp\
    ../src/utils/Strings.cpp\
    ../src/utils/ExpatParser.cpp\
    ../src/utils/ExpatUserData.cpp\
    ../src/utils/ExpatHandlers.cpp\
    ../src/utils/Utils.cpp\
    ../src/utils/Format.cpp\
    ../src/utils/Thread.cpp\
    \
    ../src/kernel/MonteCarlo.hpp\
    ../src/kernel/SimulationThread.hpp\
    ../src/kernel/SimulatedData.hpp\
    ../src/kernel/IData.hpp\
    ../src/kernel/Aggregator.hpp\
    ../src/params/Params.hpp\
    ../src/params/Interest.hpp\
    ../src/params/Ratings.hpp\
    ../src/params/Transitions.hpp\
    ../src/params/Survivals.hpp\
    ../src/params/Sectors.hpp\
    ../src/params/Correlations.hpp\
    ../src/params/Segmentations.hpp\
    ../src/params/Segmentation.hpp\
    ../src/params/Defaults.hpp\
    ../src/math/PowMatrix.hpp\
    ../src/math/Copula.hpp\
    ../src/math/CopulaCalibration.hpp\
    ../src/math/BlockMatrixChol.hpp\
    ../src/math/BlockMatrixCholInv.hpp\
    ../src/math/BlockGaussianCopula.hpp\
    ../src/math/BlockTStudentCopula.hpp\
    ../src/math/FastTStudentCdf.hpp\
    ../src/portfolio/Portfolio.hpp\
    ../src/portfolio/Obligor.hpp\
    ../src/portfolio/Asset.hpp\
    ../src/portfolio/DateValues.hpp\
    ../src/portfolio/Recovery.hpp\
    ../src/portfolio/Exposure.hpp\
    ../src/utils/Exception.hpp\
    ../src/utils/Logger.hpp\
    ../src/utils/Parser.hpp\
    ../src/utils/File.hpp\
    ../src/utils/Date.hpp\
    ../src/utils/Timer.hpp\
    ../src/utils/Arrays.hpp\
    ../src/utils/Strings.hpp\
    ../src/utils/ExpatParser.hpp\
    ../src/utils/ExpatUserData.hpp\
    ../src/utils/ExpatHandlers.hpp\
    ../src/utils/Utils.hpp\
    ../src/utils/Format.hpp\
    ../src/utils/Thread.hpp\
    ../src/utils/config.h

ccruncher_CXXFLAGS =
ccruncher_LDADD =
ccruncher_LDFLAGS =

# -------------------------------------------------------------
# tests
# -------------------------------------------------------------
tests_SOURCES = \
    ../deps/MiniCppUnit-2.5/MiniCppUnit.cxx\
    ../deps/MiniCppUnit-2.5/TestsRunner.cxx\
    ../deps/gzstream-1.5/gzstream.cpp\
    ../src/utils/ArraysTest.cpp\
    ../src/utils/StringsTest.cpp\
    ../src/utils/TimerTest.cpp\
    ../src/utils/DateTest.cpp\
    ../src/utils/ParserTest.cpp\
    ../src/utils/ExceptionTest.cpp\
    ../src/utils/FileTest.cpp\
    ../src/utils/FormatTest.cpp\
    ../src/utils/UtilsTest.cpp\
    ../src/utils/ExpatUserDataTest.cpp\
    ../src/math/PowMatrixTest.cpp\
    ../src/math/BlockMatrixCholTest.cpp\
    ../src/math/BlockGaussianCopulaTest.cpp\
    ../src/math/BlockTStudentCopulaTest.cpp\
    ../src/math/FastTStudentCdfTest.cpp\
    ../src/portfolio/AssetTest.cpp\
    ../src/portfolio/DateValuesTest.cpp\
    ../src/portfolio/RecoveryTest.cpp\
    ../src/portfolio/ExposureTest.cpp\
    ../src/portfolio/ObligorTest.cpp\
    ../src/params/InterestTest.cpp\
    ../src/params/SegmentationTest.cpp\
    ../src/params/SegmentationsTest.cpp\
    ../src/params/ParamsTest.cpp\
    ../src/params/CorrelationsTest.cpp\
    ../src/params/RatingsTest.cpp\
    ../src/params/SectorsTest.cpp\
    ../src/params/TransitionsTest.cpp\
    ../src/params/SurvivalsTest.cpp\
    ../src/kernel/IDataTest.cpp\
    ../src/kernel/SimulatedDataTest.cpp\
    \
    ../src/utils/Arrays.cpp\
    ../src/utils/Strings.cpp\
    ../src/utils/Timer.cpp\
    ../src/utils/Parser.cpp\
    ../src/utils/Logger.cpp\
    ../src/utils/Date.cpp\
    ../src/utils/Exception.cpp\
    ../src/utils/File.cpp\
    ../src/utils/ExpatParser.cpp\
    ../src/utils/ExpatUserData.cpp\
    ../src/utils/ExpatHandlers.cpp\
    ../src/utils/Utils.cpp\
    ../src/utils/Format.cpp\
    ../src/utils/Thread.cpp\
    ../src/math/PowMatrix.cpp\
    ../src/math/CopulaCalibration.cpp\
    ../src/math/BlockMatrixChol.cpp\
    ../src/math/BlockMatrixCholInv.cpp\
    ../src/math/BlockGaussianCopula.cpp\
    ../src/math/BlockTStudentCopula.cpp\
    ../src/math/FastTStudentCdf.cpp\
    ../src/portfolio/Portfolio.cpp\
    ../src/portfolio/Obligor.cpp\
    ../src/portfolio/Asset.cpp\
    ../src/portfolio/DateValues.cpp\
    ../src/portfolio/Recovery.cpp\
    ../src/portfolio/Exposure.cpp\
    ../src/params/Sectors.cpp\
    ../src/params/Ratings.cpp\
    ../src/params/Params.cpp\
    ../src/params/Transitions.cpp\
    ../src/params/Correlations.cpp\
    ../src/params/Interest.cpp\
    ../src/params/Segmentations.cpp\
    ../src/params/Segmentation.cpp\
    ../src/params/Survivals.cpp\
    ../src/params/Defaults.cpp\
    ../src/kernel/MonteCarlo.cpp\
    ../src/kernel/SimulationThread.cpp\
    ../src/kernel/SimulatedData.cpp\
    ../src/kernel/IData.cpp\
    ../src/kernel/Aggregator.cpp\
    \
    ../src/utils/ArraysTest.hpp\
    ../src/utils/StringsTest.hpp\
    ../src/utils/TimerTest.hpp\
    ../src/utils/DateTest.hpp\
    ../src/utils/ParserTest.hpp\
    ../src/utils/ExceptionTest.hpp\
    ../src/utils/FileTest.hpp\
    ../src/utils/FormatTest.hpp\
    ../src/utils/UtilsTest.hpp\
    ../src/utils/ExpatUserDataTest.hpp\
    ../src/math/PowMatrixTest.hpp\
    ../src/math/BlockMatrixCholTest.hpp\
    ../src/math/BlockGaussianCopulaTest.hpp\
    ../src/math/BlockTStudentCopulaTest.hpp\
    ../src/math/FastTStudentCdfTest.hpp\
    ../src/portfolio/AssetTest.hpp\
    ../src/portfolio/DateValuesTest.hpp\
    ../src/portfolio/RecoveryTest.hpp\
    ../src/portfolio/ExposureTest.hpp\
    ../src/portfolio/ObligorTest.hpp\
    ../src/params/CorrelationsTest.hpp\
    ../src/params/InterestTest.hpp\
    ../src/params/SegmentationTest.hpp\
    ../src/params/SegmentationsTest.hpp\
    ../src/params/ParamsTest.hpp\
    ../src/params/RatingsTest.hpp\
    ../src/params/SectorsTest.hpp\
    ../src/params/TransitionsTest.hpp\
    ../src/params/SurvivalsTest.hpp\
    ../src/kernel/IDataTest.hpp\
    ../src/kernel/SimulatedDataTest.hpp\
    \
    ../src/utils/Arrays.hpp\
    ../src/utils/Strings.hpp\
    ../src/utils/Timer.hpp\
    ../src/utils/Parser.hpp\
    ../src/utils/Logger.hpp\
    ../src/utils/Date.hpp\
    ../src/utils/Exception.hpp\
    ../src/utils/File.hpp\
    ../src/utils/ExpatParser.hpp\
    ../src/utils/ExpatUserData.hpp\
    ../src/utils/ExpatHandlers.hpp\
    ../src/utils/Utils.hpp\
    ../src/utils/Format.hpp\
    ../src/utils/Thread.hpp\
    ../src/math/PowMatrix.hpp\
    ../src/math/Copula.hpp\
    ../src/math/CopulaCalibration.hpp\
    ../src/math/BlockMatrixChol.hpp\
    ../src/math/BlockMatrixCholInv.hpp\
    ../src/math/BlockGaussianCopula.hpp\
    ../src/math/BlockTStudentCopula.hpp\
    ../src/math/FastTStudentCdf.hpp\
    ../src/portfolio/Asset.hpp\
    ../src/portfolio/DateValues.hpp\
    ../src/portfolio/Recovery.hpp\
    ../src/portfolio/Exposure.hpp\
    ../src/portfolio/Portfolio.hpp\
    ../src/portfolio/Obligor.hpp\
    ../src/params/Sectors.hpp\
    ../src/params/Ratings.hpp\
    ../src/params/Params.hpp\
    ../src/params/Transitions.hpp\
    ../src/params/Correlations.hpp\
    ../src/params/Interest.hpp\
    ../src/params/Segmentations.hpp\
    ../src/params/Segmentation.hpp\
    ../src/params/Survivals.hpp\
    ../src/params/Defaults.hpp\
    ../src/kernel/MonteCarlo.hpp\
    ../src/kernel/SimulationThread.hpp\
    ../src/kernel/SimulatedData.hpp\
    ../src/kernel/IData.hpp\
    ../src/kernel/Aggregator.hpp\
    ../src/utils/config.h

tests_CXXFLAGS =
tests_LDADD =
tests_LDFLAGS =

# -------------------------------------------------------------
# samplegen
# -------------------------------------------------------------
samplegen_SOURCES = \
    ../deps/gzstream-1.5/gzstream.cpp\
    ../src/utils/Exception.cpp\
    ../src/utils/Logger.cpp\
    ../src/utils/Parser.cpp\
    ../src/utils/File.cpp\
    ../src/utils/Date.cpp\
    ../src/utils/Timer.cpp\
    ../src/utils/Arrays.cpp\
    ../src/utils/Strings.cpp\
    ../src/utils/ExpatParser.cpp\
    ../src/utils/ExpatUserData.cpp\
    ../src/utils/ExpatHandlers.cpp\
    ../src/utils/Utils.cpp\
    ../src/utils/Format.cpp\
    ../src/utils/Thread.cpp\
    ../src/math/PowMatrix.cpp\
    ../src/math/CopulaCalibration.cpp\
    ../src/math/BlockMatrixChol.cpp\
    ../src/math/BlockMatrixCholInv.cpp\
    ../src/math/BlockGaussianCopula.cpp\
    ../src/math/BlockTStudentCopula.cpp\
    ../src/math/FastTStudentCdf.cpp\
    ../src/portfolio/Asset.cpp\
    ../src/portfolio/DateValues.cpp\
    ../src/portfolio/Recovery.cpp\
    ../src/portfolio/Exposure.cpp\
    ../src/portfolio/Portfolio.cpp\
    ../src/portfolio/Obligor.cpp\
    ../src/params/Params.cpp\
    ../src/params/Interest.cpp\
    ../src/params/Ratings.cpp\
    ../src/params/Transitions.cpp\
    ../src/params/Survivals.cpp\
    ../src/params/Sectors.cpp\
    ../src/params/Correlations.cpp\
    ../src/params/Segmentations.cpp\
    ../src/params/Segmentation.cpp\
    ../src/params/Defaults.cpp\
    ../src/kernel/samplegen.cpp\
    ../src/kernel/MonteCarlo.cpp\
    ../src/kernel/SimulationThread.cpp\
    ../src/kernel/SimulatedData.cpp\
    ../src/kernel/IData.cpp\
    ../src/kernel/Aggregator.cpp\
    \
    ../src/utils/Exception.hpp\
    ../src/utils/Logger.hpp\
    ../src/utils/Parser.hpp\
    ../src/utils/File.hpp\
    ../src/utils/Date.hpp\
    ../src/utils/Timer.hpp\
    ../src/utils/Arrays.hpp\
    ../src/utils/Strings.hpp\
    ../src/utils/ExpatParser.hpp\
    ../src/utils/ExpatUserData.hpp\
    ../src/utils/ExpatHandlers.hpp\
    ../src/utils/Utils.hpp\
    ../src/utils/Format.hpp\
    ../src/utils/Thread.hpp\
    ../src/math/PowMatrix.hpp\
    ../src/math/Copula.hpp\
    ../src/math/CopulaCalibration.hpp\
    ../src/math/BlockMatrixChol.hpp\
    ../src/math/BlockMatrixCholInv.hpp\
    ../src/math/BlockGaussianCopula.hpp\
    ../src/math/BlockTStudentCopula.hpp\
    ../src/math/FastTStudentCdf.hpp\
    ../src/portfolio/Portfolio.hpp\
    ../src/portfolio/Obligor.hpp\
    ../src/portfolio/Asset.hpp\
    ../src/portfolio/DateValues.hpp\
    ../src/portfolio/Recovery.hpp\
    ../src/portfolio/Exposure.hpp\
    ../src/params/Params.hpp\
    ../src/params/Interest.hpp\
    ../src/params/Ratings.hpp\
    ../src/params/Transitions.hpp\
    ../src/params/Survivals.hpp\
    ../src/params/Sectors.hpp\
    ../src/params/Correlations.hpp\
    ../src/params/Segmentations.hpp\
    ../src/params/Segmentation.hpp\
    ../src/params/Defaults.hpp\
    ../src/kernel/MonteCarlo.hpp\
    ../src/kernel/SimulationThread.hpp\
    ../src/kernel/SimulatedData.hpp\
    ../src/kernel/IData.hpp\
    ../src/kernel/Aggregator.hpp\
    ../src/utils/config.h

samplegen_CXXFLAGS =
samplegen_LDADD =
samplegen_LDFLAGS =

