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
    ../utils/XMLUtils.cpp\
    ../utils/Utils.cpp\
    ../utils/Date.cpp\
    ../utils/Parser.cpp\
    ../utils/Exception.cpp\
    SAX2Count.cpp\
    SAX2Handlers.cpp

tests_CXXFLAGS = -Wall -W -Wshadow -pg

tests_LDFLAGS = -L/home/horizon/temp/creditcruncher/share/xercesc-2.6/lib

tests_LDADD = -lm -ldl -lxerces-c
    
# -------------------------------------------------------------
