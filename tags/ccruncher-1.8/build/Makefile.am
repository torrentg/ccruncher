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
    ../src/portfolio/Portfolio.cpp\
    ../src/portfolio/Obligor.cpp\
    ../src/params/Params.cpp\
    ../src/interests/Interest.cpp\
    ../src/interests/Rate.cpp\
    ../src/ratings/Rating.cpp\
    ../src/ratings/Ratings.cpp\
    ../src/transitions/TransitionMatrix.cpp\
    ../src/survival/Survival.cpp\
    ../src/sectors/Sector.cpp\
    ../src/sectors/Sectors.cpp\
    ../src/correlations/CorrelationMatrix.cpp\
    ../src/math/PowMatrix.cpp\
    ../src/math/BlockMatrixChol.cpp\
    ../src/math/BlockGaussianCopula.cpp\
    ../src/math/BlockTStudentCopula.cpp\
    ../src/portfolio/Asset.cpp\
    ../src/portfolio/DateValues.cpp\
    ../src/portfolio/Recovery.cpp\
    ../src/portfolio/Exposure.cpp\
    ../src/segmentations/Segmentations.cpp\
    ../src/segmentations/Segmentation.cpp\
    ../src/utils/Exception.cpp\
    ../src/utils/Logger.cpp\
    ../src/utils/Parser.cpp\
    ../src/utils/File.cpp\
    ../src/utils/Date.cpp\
    ../src/utils/Timer.cpp\
    ../src/utils/Arrays.cpp\
    ../src/utils/LookupTable.cpp\
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
    ../src/portfolio/Portfolio.hpp\
    ../src/portfolio/Obligor.hpp\
    ../src/params/Params.hpp\
    ../src/interests/Interest.hpp\
    ../src/interests/Rate.hpp\
    ../src/ratings/Rating.hpp\
    ../src/ratings/Ratings.hpp\
    ../src/transitions/TransitionMatrix.hpp\
    ../src/survival/Survival.hpp\
    ../src/sectors/Sector.hpp\
    ../src/sectors/Sectors.hpp\
    ../src/correlations/CorrelationMatrix.hpp\
    ../src/math/PowMatrix.hpp\
    ../src/math/Copula.hpp\
    ../src/math/BlockMatrixChol.hpp\
    ../src/math/BlockGaussianCopula.hpp\
    ../src/math/BlockTStudentCopula.hpp\
    ../src/portfolio/Asset.hpp\
    ../src/portfolio/DateValues.hpp\
    ../src/portfolio/Recovery.hpp\
    ../src/portfolio/Exposure.hpp\
    ../src/segmentations/Segmentations.hpp\
    ../src/segmentations/Segmentation.hpp\
    ../src/utils/Exception.hpp\
    ../src/utils/Logger.hpp\
    ../src/utils/Parser.hpp\
    ../src/utils/File.hpp\
    ../src/utils/Date.hpp\
    ../src/utils/Timer.hpp\
    ../src/utils/Arrays.hpp\
    ../src/utils/LookupTable.hpp\
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
    ../src/utils/LookupTableTest.cpp\
    ../src/utils/StringsTest.cpp\
    ../src/utils/TimerTest.cpp\
    ../src/utils/DateTest.cpp\
    ../src/utils/ParserTest.cpp\
    ../src/utils/ExceptionTest.cpp\
    ../src/utils/FileTest.cpp\
    ../src/utils/FormatTest.cpp\
    ../src/utils/UtilsTest.cpp\
    ../src/math/PowMatrixTest.cpp\
    ../src/math/BlockMatrixCholTest.cpp\
    ../src/math/BlockGaussianCopulaTest.cpp\
    ../src/math/BlockTStudentCopulaTest.cpp\
    ../src/correlations/CorrelationMatrixTest.cpp\
    ../src/portfolio/AssetTest.cpp\
    ../src/portfolio/DateValuesTest.cpp\
    ../src/portfolio/RecoveryTest.cpp\
    ../src/portfolio/ExposureTest.cpp\
    ../src/interests/InterestTest.cpp\
    ../src/segmentations/SegmentationTest.cpp\
    ../src/segmentations/SegmentationsTest.cpp\
    ../src/kernel/IDataTest.cpp\
    ../src/kernel/SimulatedDataTest.cpp\
    ../src/params/ParamsTest.cpp\
    ../src/portfolio/ObligorTest.cpp\
    ../src/ratings/RatingsTest.cpp\
    ../src/sectors/SectorsTest.cpp\
    ../src/transitions/TransitionMatrixTest.cpp\
    ../src/survival/SurvivalTest.cpp\
    ../src/portfolio/Asset.cpp\
    ../src/portfolio/DateValues.cpp\
    ../src/portfolio/Recovery.cpp\
    ../src/portfolio/Exposure.cpp\
    ../src/utils/Arrays.cpp\
    ../src/utils/LookupTable.cpp\
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
    ../src/math/BlockMatrixChol.cpp\
    ../src/math/BlockGaussianCopula.cpp\
    ../src/math/BlockTStudentCopula.cpp\
    ../src/kernel/MonteCarlo.cpp\
    ../src/kernel/SimulationThread.cpp\
    ../src/kernel/SimulatedData.cpp\
    ../src/kernel/IData.cpp\
    ../src/kernel/Aggregator.cpp\
    ../src/sectors/Sector.cpp\
    ../src/sectors/Sectors.cpp\
    ../src/ratings/Rating.cpp\
    ../src/ratings/Ratings.cpp\
    ../src/params/Params.cpp\
    ../src/transitions/TransitionMatrix.cpp\
    ../src/correlations/CorrelationMatrix.cpp\
    ../src/portfolio/Portfolio.cpp\
    ../src/portfolio/Obligor.cpp\
    ../src/interests/Rate.cpp\
    ../src/interests/Interest.cpp\
    ../src/segmentations/Segmentations.cpp\
    ../src/segmentations/Segmentation.cpp\
    ../src/survival/Survival.cpp\
    \
    ../src/utils/ArraysTest.hpp\
    ../src/utils/LookupTableTest.hpp\
    ../src/utils/StringsTest.hpp\
    ../src/utils/TimerTest.hpp\
    ../src/utils/DateTest.hpp\
    ../src/utils/ParserTest.hpp\
    ../src/utils/ExceptionTest.hpp\
    ../src/utils/FileTest.hpp\
    ../src/utils/FormatTest.hpp\
    ../src/utils/UtilsTest.hpp\
    ../src/math/Copula.hpp\
    ../src/math/PowMatrixTest.hpp\
    ../src/math/BlockMatrixCholTest.hpp\
    ../src/math/BlockGaussianCopulaTest.hpp\
    ../src/math/BlockTStudentCopulaTest.hpp\
    ../src/correlations/CorrelationMatrixTest.hpp\
    ../src/portfolio/AssetTest.hpp\
    ../src/portfolio/DateValuesTest.hpp\
    ../src/portfolio/RecoveryTest.hpp\
    ../src/portfolio/ExposureTest.hpp\
    ../src/interests/InterestTest.hpp\
    ../src/segmentations/SegmentationTest.hpp\
    ../src/segmentations/SegmentationsTest.hpp\
    ../src/kernel/IDataTest.hpp\
    ../src/kernel/SimulatedDataTest.hpp\
    ../src/params/ParamsTest.hpp\
    ../src/portfolio/ObligorTest.hpp\
    ../src/ratings/RatingsTest.hpp\
    ../src/sectors/SectorsTest.hpp\
    ../src/transitions/TransitionMatrixTest.hpp\
    ../src/survival/SurvivalTest.hpp\
    ../src/portfolio/Asset.hpp\
    ../src/portfolio/DateValues.hpp\
    ../src/portfolio/Recovery.hpp\
    ../src/portfolio/Exposure.hpp\
    ../src/utils/Arrays.hpp\
    ../src/utils/LookupTable.hpp\
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
    ../src/math/BlockMatrixChol.hpp\
    ../src/math/BlockGaussianCopula.hpp\
    ../src/math/BlockTStudentCopula.hpp\
    ../src/kernel/MonteCarlo.hpp\
    ../src/kernel/SimulationThread.hpp\
    ../src/kernel/SimulatedData.hpp\
    ../src/kernel/IData.hpp\
    ../src/kernel/Aggregator.hpp\
    ../src/sectors/Sector.hpp\
    ../src/sectors/Sectors.hpp\
    ../src/ratings/Rating.hpp\
    ../src/ratings/Ratings.hpp\
    ../src/params/Params.hpp\
    ../src/transitions/TransitionMatrix.hpp\
    ../src/correlations/CorrelationMatrix.hpp\
    ../src/portfolio/Portfolio.hpp\
    ../src/portfolio/Obligor.hpp\
    ../src/interests/Rate.hpp\
    ../src/interests/Interest.hpp\
    ../src/segmentations/Segmentations.hpp\
    ../src/segmentations/Segmentation.hpp\
    ../src/survival/Survival.hpp\
    ../src/utils/config.h

tests_CXXFLAGS =
tests_LDADD =
tests_LDFLAGS =

# -------------------------------------------------------------
# samplegen
# -------------------------------------------------------------
samplegen_SOURCES = \
    ../deps/gzstream-1.5/gzstream.cpp\
    ../src/kernel/samplegen.cpp\
    ../src/kernel/MonteCarlo.cpp\
    ../src/kernel/SimulationThread.cpp\
    ../src/kernel/SimulatedData.cpp\
    ../src/kernel/IData.cpp\
    ../src/kernel/Aggregator.cpp\
    ../src/portfolio/Portfolio.cpp\
    ../src/portfolio/Obligor.cpp\
    ../src/params/Params.cpp\
    ../src/interests/Interest.cpp\
    ../src/interests/Rate.cpp\
    ../src/ratings/Rating.cpp\
    ../src/ratings/Ratings.cpp\
    ../src/transitions/TransitionMatrix.cpp\
    ../src/survival/Survival.cpp\
    ../src/sectors/Sector.cpp\
    ../src/sectors/Sectors.cpp\
    ../src/correlations/CorrelationMatrix.cpp\
    ../src/math/PowMatrix.cpp\
    ../src/math/BlockMatrixChol.cpp\
    ../src/math/BlockGaussianCopula.cpp\
    ../src/math/BlockTStudentCopula.cpp\
    ../src/portfolio/Asset.cpp\
    ../src/portfolio/DateValues.cpp\
    ../src/portfolio/Recovery.cpp\
    ../src/portfolio/Exposure.cpp\
    ../src/segmentations/Segmentations.cpp\
    ../src/segmentations/Segmentation.cpp\
    ../src/utils/Exception.cpp\
    ../src/utils/Logger.cpp\
    ../src/utils/Parser.cpp\
    ../src/utils/File.cpp\
    ../src/utils/Date.cpp\
    ../src/utils/Timer.cpp\
    ../src/utils/Arrays.cpp\
    ../src/utils/LookupTable.cpp\
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
    ../src/portfolio/Portfolio.hpp\
    ../src/portfolio/Obligor.hpp\
    ../src/params/Params.hpp\
    ../src/interests/Interest.hpp\
    ../src/interests/Rate.hpp\
    ../src/ratings/Rating.hpp\
    ../src/ratings/Ratings.hpp\
    ../src/transitions/TransitionMatrix.hpp\
    ../src/survival/Survival.hpp\
    ../src/sectors/Sector.hpp\
    ../src/sectors/Sectors.hpp\
    ../src/correlations/CorrelationMatrix.hpp\
    ../src/math/PowMatrix.hpp\
    ../src/math/Copula.hpp\
    ../src/math/BlockMatrixChol.hpp\
    ../src/math/BlockGaussianCopula.hpp\
    ../src/math/BlockTStudentCopula.hpp\
    ../src/portfolio/Asset.hpp\
    ../src/portfolio/DateValues.hpp\
    ../src/portfolio/Recovery.hpp\
    ../src/portfolio/Exposure.hpp\
    ../src/segmentations/Segmentations.hpp\
    ../src/segmentations/Segmentation.hpp\
    ../src/utils/Exception.hpp\
    ../src/utils/Logger.hpp\
    ../src/utils/Parser.hpp\
    ../src/utils/File.hpp\
    ../src/utils/Date.hpp\
    ../src/utils/Timer.hpp\
    ../src/utils/Arrays.hpp\
    ../src/utils/LookupTable.hpp\
    ../src/utils/Strings.hpp\
    ../src/utils/ExpatParser.hpp\
    ../src/utils/ExpatUserData.hpp\
    ../src/utils/ExpatHandlers.hpp\
    ../src/utils/Utils.hpp\
    ../src/utils/Format.hpp\
    ../src/utils/Thread.hpp\
    ../src/utils/config.h

samplegen_CXXFLAGS =
samplegen_LDADD =
samplegen_LDFLAGS =

