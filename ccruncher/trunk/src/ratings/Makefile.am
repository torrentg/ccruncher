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
    RatingsTest.cpp\
    Ratings.cpp\
    Rating.cpp\
    DateRatings.cpp\
    ../utils/Exception.cpp\
    ../utils/XMLUtils.cpp\
    ../utils/Parser.cpp\
    ../utils/Date.cpp\
    ../utils/Utils.cpp

tests_CXXFLAGS = -Wall -W -Wshadow

tests_LDFLAGS = -L../../share/xercesc-2.6/lib

tests_LDADD = -lm -ldl -lxerces-c
    
# -------------------------------------------------------------
