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
    DateTest.cpp\
    ExceptionTest.cpp\
    ParserTest.cpp\
    TimerTest.cpp\
    UtilsTest.cpp\
    XMLUtilsTest.cpp\
    FileTest.cpp\
    Date.cpp\
    Exception.cpp\
    Parser.cpp\
    Timer.cpp\
    Utils.cpp\
    XMLUtils.cpp\
    File.cpp

tests_CXXFLAGS = -Wall -W -Wshadow

tests_LDFLAGS = -L/home/horizon/temp/creditcruncher/share/xercesc-2.6/lib

tests_LDADD = -lm -ldl -lxerces-c
    
# -------------------------------------------------------------
