# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
    -I../share/jama-1.2.2/\
    -I../share/Mersenne-1.0/\
    -I../share/MiniCppUnit-2.2/

bin_PROGRAMS = ccruncher tabulate generator trimmer

check_PROGRAMS = tests

# -------------------------------------------------------------
# ccruncher
# -------------------------------------------------------------
ccruncher_SOURCES = \
    ./kernel/ccruncher.cpp\
    ./kernel/MonteCarlo.cpp\
    ./kernel/IData.cpp\
    ./portfolio/Portfolio.cpp\
    ./portfolio/Client.cpp\
    ./params/Params.cpp\
    ./interests/Interests.cpp\
    ./interests/Interest.cpp\
    ./interests/Rate.cpp\
    ./ratings/Rating.cpp\
    ./ratings/Ratings.cpp\
    ./ratings/DateRatings.cpp\
    ./transitions/TransitionMatrix.cpp\
    ./sectors/Sector.cpp\
    ./sectors/Sectors.cpp\
    ./correlations/CorrelationMatrix.cpp\
    ./math/CholeskyDecomposition.cpp\
    ./math/PowMatrix.cpp\
    ./math/CopulaNormal.cpp\
    ./math/Normal.cpp\
    ./finances/Asset.cpp\
    ./finances/Bond.cpp\
    ./finances/DateValues.cpp\
    ./segmentations/Segmentations.cpp\
    ./segmentations/Segmentation.cpp\
    ./segmentations/Segment.cpp\
    ./aggregators/SegmentAggregator.cpp\
    ./aggregators/Aggregators.cpp\
    ./aggregators/Aggregator.cpp\
    ./utils/Exception.cpp\
    ./utils/XMLUtils.cpp\
    ./utils/Parser.cpp\
    ./utils/File.cpp\
    ./utils/Date.cpp\
    ./utils/Timer.cpp\
    ./utils/Utils.cpp\
    \
    ./kernel/MonteCarlo.hpp\
    ./kernel/IData.hpp\
    ./portfolio/Portfolio.hpp\
    ./portfolio/Client.hpp\
    ./params/Params.hpp\
    ./interests/Interests.hpp\
    ./interests/Interest.hpp\
    ./interests/Rate.hpp\
    ./ratings/Rating.hpp\
    ./ratings/Ratings.hpp\
    ./ratings/DateRatings.hpp\
    ./transitions/TransitionMatrix.hpp\
    ./sectors/Sector.hpp\
    ./sectors/Sectors.hpp\
    ./correlations/CorrelationMatrix.hpp\
    ./math/CholeskyDecomposition.hpp\
    ./math/PowMatrix.hpp\
    ./math/Copula.hpp\
    ./math/CopulaNormal.hpp\
    ./math/Normal.hpp\
    ./finances/Asset.hpp\
    ./finances/Bond.hpp\
    ./finances/DateValues.hpp\
    ./segmentations/Segmentations.hpp\
    ./segmentations/Segmentation.hpp\
    ./segmentations/Segment.hpp\
    ./aggregators/SegmentAggregator.hpp\
    ./aggregators/Aggregators.hpp\
    ./aggregators/Aggregator.hpp\
    ./utils/Exception.hpp\
    ./utils/XMLUtils.hpp\
    ./utils/Parser.hpp\
    ./utils/File.hpp\
    ./utils/Date.hpp\
    ./utils/Timer.hpp\
    ./utils/Utils.hpp

ccruncher_CXXFLAGS = -Wall -W -Wshadow
ccruncher_LDFLAGS =
ccruncher_LDADD = -lm -ldl -lxerces-c

# -------------------------------------------------------------
# trimmer
# -------------------------------------------------------------
trimmer_SOURCES = \
    ./tools/trimmer.cpp

trimmer_CXXFLAGS = -Wall -W -Wshadow
trimmer_LDFLAGS = 
trimmer_LDADD = 

# -------------------------------------------------------------
# tabulate
# -------------------------------------------------------------
tabulate_SOURCES = \
    ./tools/tabulate.cpp\
    ./tools/SAX2Handlers.cpp\
    ./utils/Exception.cpp\
    ./utils/XMLUtils.cpp\
    ./utils/Parser.cpp\
    ./utils/Date.cpp\
    ./utils/File.cpp\
    ./utils/Utils.cpp\
    \
    ./tools/SAX2Handlers.hpp\
    ./utils/Exception.hpp\
    ./utils/XMLUtils.hpp\
    ./utils/Parser.hpp\
    ./utils/Date.hpp\
    ./utils/File.hpp\
    ./utils/Utils.hpp

tabulate_CXXFLAGS = -Wall -W -Wshadow
tabulate_LDFLAGS =
tabulate_LDADD = -ldl -lxerces-c

# -------------------------------------------------------------
# generator
# -------------------------------------------------------------
generator_SOURCES = \
    ./tools/generator.cpp\
    ./kernel/IData.cpp\
    ./portfolio/Portfolio.cpp\
    ./portfolio/Client.cpp\
    ./params/Params.cpp\
    ./interests/Interests.cpp\
    ./interests/Interest.cpp\
    ./interests/Rate.cpp\
    ./ratings/Rating.cpp\
    ./ratings/Ratings.cpp\
    ./ratings/DateRatings.cpp\
    ./transitions/TransitionMatrix.cpp\
    ./sectors/Sector.cpp\
    ./sectors/Sectors.cpp\
    ./correlations/CorrelationMatrix.cpp\
    ./math/CholeskyDecomposition.cpp\
    ./math/PowMatrix.cpp\
    ./math/CopulaNormal.cpp\
    ./math/Normal.cpp\
    ./finances/Asset.cpp\
    ./finances/Bond.cpp\
    ./finances/DateValues.cpp\
    ./segmentations/Segmentations.cpp\
    ./segmentations/Segmentation.cpp\
    ./segmentations/Segment.cpp\
    ./aggregators/SegmentAggregator.cpp\
    ./aggregators/Aggregators.cpp\
    ./aggregators/Aggregator.cpp\
    ./utils/Exception.cpp\
    ./utils/XMLUtils.cpp\
    ./utils/Parser.cpp\
    ./utils/File.cpp\
    ./utils/Date.cpp\
    ./utils/Timer.cpp\
    ./utils/Utils.cpp\
    \
    ./kernel/IData.hpp\
    ./portfolio/Portfolio.hpp\
    ./portfolio/Client.hpp\
    ./params/Params.hpp\
    ./interests/Interests.hpp\
    ./interests/Interest.hpp\
    ./interests/Rate.hpp\
    ./ratings/Rating.hpp\
    ./ratings/Ratings.hpp\
    ./ratings/DateRatings.hpp\
    ./transitions/TransitionMatrix.hpp\
    ./sectors/Sector.hpp\
    ./sectors/Sectors.hpp\
    ./correlations/CorrelationMatrix.hpp\
    ./math/CholeskyDecomposition.hpp\
    ./math/Copula.hpp\
    ./math/PowMatrix.hpp\
    ./math/CopulaNormal.hpp\
    ./math/Normal.hpp\
    ./finances/Asset.hpp\
    ./finances/Bond.hpp\
    ./finances/DateValues.hpp\
    ./segmentations/Segmentations.hpp\
    ./segmentations/Segmentation.hpp\
    ./segmentations/Segment.hpp\
    ./aggregators/SegmentAggregator.hpp\
    ./aggregators/Aggregators.hpp\
    ./aggregators/Aggregator.hpp\
    ./utils/Exception.hpp\
    ./utils/XMLUtils.hpp\
    ./utils/Parser.hpp\
    ./utils/File.hpp\
    ./utils/Date.hpp\
    ./utils/Timer.hpp\
    ./utils/Utils.hpp

generator_CXXFLAGS = -Wall -W -Wshadow
generator_LDFLAGS =
generator_LDADD = -lm -ldl -lxerces-c

# -------------------------------------------------------------
# tests
# -------------------------------------------------------------
tests_SOURCES = \
    ../share/MiniCppUnit-2.2/MiniCppUnit.cxx\
    ../share/MiniCppUnit-2.2/TestsRunner.cxx\
    ./utils/XMLUtilsTest.cpp\
    ./utils/UtilsTest.cpp\
    ./utils/TimerTest.cpp\
    ./utils/DateTest.cpp\
    ./utils/ParserTest.cpp\
    ./utils/ExceptionTest.cpp\
    ./utils/FileTest.cpp\
    ./math/PowMatrixTest.cpp\
    ./math/NormalTest.cpp\
    ./math/CopulaNormalTest.cpp\
    ./math/MersenneTwisterTest.cpp\
    ./math/CholeskyDecompositionTest.cpp\
    ./aggregators/AggregatorsTest.cpp\
    ./correlations/CorrelationMatrixTest.cpp\
    ./finances/BondTest.cpp\
    ./finances/AssetTest.cpp\
    ./interests/InterestTest.cpp\
    ./interests/InterestsTest.cpp\
    ./segmentations/SegmentationTest.cpp\
    ./segmentations/SegmentationsTest.cpp\
    ./kernel/IDataTest.cpp\
    ./params/ParamsTest.cpp\
    ./portfolio/ClientTest.cpp\
    ./ratings/RatingsTest.cpp\
    ./sectors/SectorsTest.cpp\
    ./transitions/TransitionMatrixTest.cpp\
    ./finances/Asset.cpp\
    ./finances/Bond.cpp\
    ./finances/DateValues.cpp\
    ./utils/Timer.cpp\
    ./utils/Parser.cpp\
    ./utils/Utils.cpp\
    ./utils/Date.cpp\
    ./utils/XMLUtils.cpp\
    ./utils/Exception.cpp\
    ./utils/File.cpp\
    ./math/CholeskyDecomposition.cpp\
    ./math/CopulaNormal.cpp\
    ./math/Normal.cpp\
    ./math/PowMatrix.cpp\
    ./kernel/MonteCarlo.cpp\
    ./kernel/IData.cpp\
    ./sectors/Sector.cpp\
    ./sectors/Sectors.cpp\
    ./ratings/Rating.cpp\
    ./ratings/Ratings.cpp\
    ./ratings/DateRatings.cpp\
    ./params/Params.cpp\
    ./transitions/TransitionMatrix.cpp\
    ./correlations/CorrelationMatrix.cpp\
    ./portfolio/Portfolio.cpp\
    ./portfolio/Client.cpp\
    ./interests/Rate.cpp\
    ./interests/Interest.cpp\
    ./interests/Interests.cpp\
    ./aggregators/Aggregator.cpp\
    ./aggregators/Aggregators.cpp\
    ./aggregators/SegmentAggregator.cpp\
    ./segmentations/Segmentations.cpp\
    ./segmentations/Segmentation.cpp\
    ./segmentations/Segment.cpp\
    \
    ./utils/XMLUtilsTest.hpp\
    ./utils/UtilsTest.hpp\
    ./utils/TimerTest.hpp\
    ./utils/DateTest.hpp\
    ./utils/ParserTest.hpp\
    ./utils/ExceptionTest.hpp\
    ./utils/FileTest.hpp\
    ./math/Copula.hpp\
    ./math/PowMatrixTest.hpp\
    ./math/NormalTest.hpp\
    ./math/CopulaNormalTest.hpp\
    ./math/MersenneTwisterTest.hpp\
    ./math/CholeskyDecompositionTest.hpp\
    ./aggregators/AggregatorsTest.hpp\
    ./correlations/CorrelationMatrixTest.hpp\
    ./finances/BondTest.hpp\
    ./finances/AssetTest.hpp\
    ./interests/InterestTest.hpp\
    ./interests/InterestsTest.hpp\
    ./segmentations/SegmentationTest.hpp\
    ./segmentations/SegmentationsTest.hpp\
    ./kernel/IDataTest.hpp\
    ./params/ParamsTest.hpp\
    ./portfolio/ClientTest.hpp\
    ./ratings/RatingsTest.hpp\
    ./sectors/SectorsTest.hpp\
    ./transitions/TransitionMatrixTest.hpp\
    ./finances/Asset.hpp\
    ./finances/Bond.hpp\
    ./finances/DateValues.hpp\
    ./utils/Timer.hpp\
    ./utils/Parser.hpp\
    ./utils/Utils.hpp\
    ./utils/Date.hpp\
    ./utils/XMLUtils.hpp\
    ./utils/Exception.hpp\
    ./utils/File.hpp\
    ./math/CholeskyDecomposition.hpp\
    ./math/CopulaNormal.hpp\
    ./math/Normal.hpp\
    ./math/PowMatrix.hpp\
    ./kernel/MonteCarlo.hpp\
    ./kernel/IData.hpp\
    ./sectors/Sector.hpp\
    ./sectors/Sectors.hpp\
    ./ratings/Rating.hpp\
    ./ratings/Ratings.hpp\
    ./ratings/DateRatings.hpp\
    ./params/Params.hpp\
    ./transitions/TransitionMatrix.hpp\
    ./correlations/CorrelationMatrix.hpp\
    ./portfolio/Portfolio.hpp\
    ./portfolio/Client.hpp\
    ./interests/Rate.hpp\
    ./interests/Interest.hpp\
    ./interests/Interests.hpp\
    ./aggregators/Aggregator.hpp\
    ./aggregators/Aggregators.hpp\
    ./aggregators/SegmentAggregator.hpp\
    ./segmentations/Segmentations.hpp\
    ./segmentations/Segmentation.hpp\
    ./segmentations/Segment.hpp

tests_CXXFLAGS = -Wall -W -Wshadow
tests_LDFLAGS = 
tests_LDADD = -lm -ldl -lxerces-c
