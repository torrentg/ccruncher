# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AM_CPPFLAGS = \
    -I../\
    -I../../share/Mersenne-1.0/\
    -I../../share/jama-1.2.2/\
    -I../../share/MiniCppUnit-2.2/

check_PROGRAMS = tests

# -------------------------------------------------------------

tests_SOURCES = \
    ../../share/MiniCppUnit-2.2/MiniCppUnit.cxx\
    ../../share/MiniCppUnit-2.2/TestsRunner.cxx\
    NormalTest.cpp\
    MersenneTwisterTest.cpp\
    CholeskyDecompositionTest.cpp\
    CopulaNormalTest.cpp\
    PowMatrixTest.cpp\
    Normal.cpp\
    CholeskyDecomposition.cpp\
    CopulaNormal.cpp\
    PowMatrix.cpp\
    ../utils/Exception.cpp\
    ../utils/Utils.cpp

tests_CXXFLAGS = -Wall -W -Wshadow

tests_LDFLAGS = 

tests_LDADD = -ldl -lm

# -------------------------------------------------------------
