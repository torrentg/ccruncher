# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AM_CPPFLAGS = \
    -I../\
    -I../../share/MiniCppUnit-2.2/\
    -I../../share/xercesc-2.6/include/

check_PROGRAMS = tests

# -------------------------------------------------------------

tests_SOURCES = \
    ../../share/MiniCppUnit-2.2/MiniCppUnit.cxx\
    ../../share/MiniCppUnit-2.2/TestsRunner.cxx\
    AggregatorsTest.cpp\
    Aggregators.cpp\
    Aggregator.cpp\
    SegmentAggregator.cpp\
    ../ratings/Rating.cpp\
    ../ratings/Ratings.cpp\
    ../ratings/DateRatings.cpp\
    ../finances/DateValues.cpp\
    ../finances/Asset.cpp\
    ../finances/Bond.cpp\
    ../interests/Interests.cpp\
    ../interests/Interest.cpp\
    ../interests/Rate.cpp\
    ../sectors/Sector.cpp\
    ../sectors/Sectors.cpp\
    ../portfolio/Client.cpp\
    ../segmentations/Segmentations.cpp\
    ../segmentations/Segmentation.cpp\
    ../segmentations/Segment.cpp\
    ../utils/Exception.cpp\
    ../utils/XMLUtils.cpp\
    ../utils/Parser.cpp\
    ../utils/Date.cpp\
    ../utils/File.cpp\
    ../utils/Utils.cpp

tests_CXXFLAGS = -Wall -W -Wshadow

tests_LDFLAGS = -L/home/horizon/temp/creditcruncher/share/xercesc-2.6/lib

tests_LDADD = -lm -ldl -lxerces-c
    
# -------------------------------------------------------------
