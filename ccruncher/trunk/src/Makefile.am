# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

#SUBDIRS =	utils math ratings transitions sectors correlations \
#		finances interests params portfolio segmentations \
#		aggregators kernel tools


AM_CPPFLAGS = \
    -I./\
    -I../share/jama-1.2.2/\
    -I../share/Mersenne-1.0/\
    -I../share/xercesc-2.6/include/\
    -I../share/MiniCppUnit-2.2/

bin_PROGRAMS = ccruncher tabulate generator trimmer

check_PROGRAMS = tests

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
    ./utils/Utils.cpp

ccruncher_CXXFLAGS = -Wall -W -Wshadow

ccruncher_LDFLAGS = -L../share/xercesc-2.6/lib

ccruncher_LDADD = -lm -ldl -lxerces-c

# -------------------------------------------------------------

trimmer_SOURCES = \
    ./tools/trimmer.cpp

trimmer_CXXFLAGS = -Wall -W -Wshadow

trimmer_LDFLAGS = 

trimmer_LDADD = 

# -------------------------------------------------------------

tabulate_SOURCES = \
    ./tools/tabulate.cpp\
    ./tools/SAX2Handlers.cpp\
    ./utils/Exception.cpp\
    ./utils/XMLUtils.cpp\
    ./utils/Parser.cpp\
    ./utils/Date.cpp\
    ./utils/File.cpp\
    ./utils/Utils.cpp    

tabulate_CXXFLAGS = -Wall -W -Wshadow

tabulate_LDFLAGS = -L../share/xercesc-2.6/lib

tabulate_LDADD = -ldl -lxerces-c

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
    ./utils/Utils.cpp

generator_CXXFLAGS = -Wall -W -Wshadow

generator_LDFLAGS = -L../share/xercesc-2.6/lib

generator_LDADD = -lm -ldl -lxerces-c

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
    ./segmentations/Segment.cpp

tests_CXXFLAGS = -Wall -W -Wshadow

tests_LDFLAGS = -L../share/xercesc-2.6/lib

tests_LDADD = -lm -ldl -lxerces-c
