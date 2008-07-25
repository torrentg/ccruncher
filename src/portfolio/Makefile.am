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
    ClientTest.cpp\
    Client.cpp\
    Portfolio.cpp\
    ../ratings/Rating.cpp\
    ../ratings/Ratings.cpp\
    ../sectors/Sector.cpp\
    ../sectors/Sectors.cpp\
    ../finances/Asset.cpp\
    ../finances/Bond.cpp\
    ../finances/DateValues.cpp\
    ../interests/Interest.cpp\
    ../interests/Rate.cpp\
    ../segmentations/Segmentations.cpp\
    ../segmentations/Segmentation.cpp\
    ../segmentations/Segment.cpp\
    ../utils/Exception.cpp\
    ../utils/XMLUtils.cpp\
    ../utils/Parser.cpp\
    ../utils/Date.cpp\
    ../utils/Utils.cpp

tests_CXXFLAGS = -Wall -W -Wshadow

tests_LDFLAGS = -L../../share/xercesc-2.6/lib

tests_LDADD = -lm -ldl -lxerces-c
    
# -------------------------------------------------------------
