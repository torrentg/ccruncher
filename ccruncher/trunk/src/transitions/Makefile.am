# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AM_CPPFLAGS = \
    -I../\
    -I../../share/jama-1.2.2/\
    -I../../share/MiniCppUnit-2.2/\
    -I../../share/xercesc-2.6/include/

check_PROGRAMS = tests

# -------------------------------------------------------------

tests_SOURCES = \
    ../../share/MiniCppUnit-2.2/MiniCppUnit.cxx\
    ../../share/MiniCppUnit-2.2/TestsRunner.cxx\
    TransitionMatrixTest.cpp\
    TransitionMatrix.cpp\
    ../ratings/Ratings.cpp\
    ../ratings/Rating.cpp\
    ../utils/Exception.cpp\
    ../utils/XMLUtils.cpp\
    ../utils/Parser.cpp\
    ../utils/Date.cpp\
    ../utils/Utils.cpp\
    ../math/PowMatrix.cpp\
    ../math/CholeskyDecomposition.cpp

tests_CXXFLAGS = -Wall -W -Wshadow

tests_LDFLAGS = -L/home/horizon/temp/creditcruncher/share/xercesc-2.6/lib

tests_LDADD = -lm -ldl -lxerces-c
    
# -------------------------------------------------------------
