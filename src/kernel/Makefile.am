# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AM_CPPFLAGS = \
    -I../\
    -I../../share/jama-1.2.2/\
    -I../../share/Mersenne-1.0/\
    -I../../share/MiniCppUnit-2.2/\
    -I../../share/xercesc-2.6/include/

check_PROGRAMS = tests

# -------------------------------------------------------------

tests_SOURCES = \
    ../../share/MiniCppUnit-2.2/MiniCppUnit.cxx\
    ../../share/MiniCppUnit-2.2/TestsRunner.cxx\
    IDataTest.cpp\
    IData.cpp\
    ../portfolio/Portfolio.cpp\
    ../portfolio/Client.cpp\
    ../params/Params.cpp\
    ../interests/Interests.cpp\
    ../interests/Interest.cpp\
    ../interests/Rate.cpp\
    ../ratings/Rating.cpp\
    ../ratings/Ratings.cpp\
    ../ratings/DateRatings.cpp\
    ../transitions/TransitionMatrix.cpp\
    ../sectors/Sector.cpp\
    ../sectors/Sectors.cpp\
    ../correlations/CorrelationMatrix.cpp\
    ../math/CholeskyDecomposition.cpp\
    ../math/PowMatrix.cpp\
    ../finances/Asset.cpp\
    ../finances/Bond.cpp\
    ../finances/DateValues.cpp\
    ../segmentations/Segmentations.cpp\
    ../segmentations/Segmentation.cpp\
    ../segmentations/Segment.cpp\
    ../aggregators/SegmentAggregator.cpp\
    ../aggregators/Aggregators.cpp\
    ../aggregators/Aggregator.cpp\
    ../utils/Exception.cpp\
    ../utils/XMLUtils.cpp\
    ../utils/Parser.cpp\
    ../utils/File.cpp\
    ../utils/Date.cpp\
    ../utils/Utils.cpp

tests_CXXFLAGS = -Wall -W -Wshadow

tests_LDFLAGS = -L../../share/xercesc-2.6/lib

tests_LDADD = -lm -ldl -lxerces-c
    
# -------------------------------------------------------------
