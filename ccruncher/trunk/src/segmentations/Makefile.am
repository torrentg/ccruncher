# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AM_CPPFLAGS = \
    -I../\
    -I../../share/cppunit-1.10.2/include/\
    -I../../share/xercesc-2.6/include/

check_PROGRAMS = \
    tests

# -------------------------------------------------------------

tests_SOURCES = \
    tests.cpp\
    SegmentationsTest.cpp\
    SegmentationTest.cpp\
    Segmentations.cpp\
    Segmentation.cpp\
    Segment.cpp\
    ../utils/Exception.cpp\
    ../utils/XMLUtils.cpp\
    ../utils/Parser.cpp\
    ../utils/Date.cpp\
    ../utils/Utils.cpp

tests_CXXFLAGS = $(CPPUNIT_CFLAGS) -Wall -W -Wshadow

tests_LDFLAGS = $(CPPUNIT_LIBS)\
    -L/home/horizon/temp/creditcruncher/share/cppunit-1.10.2/lib\
    -L/home/horizon/temp/creditcruncher/share/xercesc-2.6/lib

tests_LDADD = \
         -lm -ldl -lxerces-c -lcppunit
    
# -------------------------------------------------------------
