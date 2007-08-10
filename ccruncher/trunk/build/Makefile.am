# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = \
    ../share/MiniCppUnit-2.5/TestsRunner.cxx\
    ../share/MiniCppUnit-2.5/MiniCppUnit.cxx\
    ../share/MiniCppUnit-2.5/MiniCppUnit.hxx\
    ../share/Mersenne-1.0/MersenneTwister.h\
    ../share/jama-1.2.5/jama_cholesky.h\
    ../share/jama-1.2.5/jama_eig.h\
    ../share/jama-1.2.5/jama_lu.h\
    ../share/jama-1.2.5/jama_qr.h\
    ../share/jama-1.2.5/jama_svd.h\
    ../share/jama-1.2.5/tnt_array1d.h\
    ../share/jama-1.2.5/tnt_array1d_utils.h\
    ../share/jama-1.2.5/tnt_array2d.h\
    ../share/jama-1.2.5/tnt_array2d_utils.h\
    ../share/jama-1.2.5/tnt_array3d.h\
    ../share/jama-1.2.5/tnt_array3d_utils.h\
    ../share/jama-1.2.5/tnt_cmat.h\
    ../share/jama-1.2.5/tnt_fortran_array1d.h\
    ../share/jama-1.2.5/tnt_fortran_array1d_utils.h\
    ../share/jama-1.2.5/tnt_fortran_array2d.h\
    ../share/jama-1.2.5/tnt_fortran_array2d_utils.h\
    ../share/jama-1.2.5/tnt_fortran_array3d.h\
    ../share/jama-1.2.5/tnt_fortran_array3d_utils.h\
    ../share/jama-1.2.5/tnt.h\
    ../share/jama-1.2.5/tnt_i_refvec.h\
    ../share/jama-1.2.5/tnt_math_utils.h\
    ../share/jama-1.2.5/tnt_sparse_matrix_csr.h\
    ../share/jama-1.2.5/tnt_stopwatch.h\
    ../share/jama-1.2.5/tnt_subscript.h\
    ../share/jama-1.2.5/tnt_vec.h\
    ../share/jama-1.2.5/tnt_version.h\
    ../share/gzstream-1.5/gzstream.h\
    ../share/gzstream-1.5/gzstream.cpp

bin_PROGRAMS = ccruncher

check_PROGRAMS = tests
#samplegen

TESTS = tests

# -------------------------------------------------------------
# ccruncher
# -------------------------------------------------------------
ccruncher_SOURCES = \
    ../share/gzstream-1.5/gzstream.cpp\
    ../src/kernel/ccruncher.cpp\
    ../src/kernel/MonteCarlo.cpp\
    ../src/kernel/IData.cpp\
    ../src/kernel/SegmentAggregator.cpp\
    ../src/portfolio/Portfolio.cpp\
    ../src/portfolio/Borrower.cpp\
    ../src/params/Params.cpp\
    ../src/interests/Interests.cpp\
    ../src/interests/Interest.cpp\
    ../src/interests/Rate.cpp\
    ../src/ratings/Rating.cpp\
    ../src/ratings/Ratings.cpp\
    ../src/transitions/TransitionMatrix.cpp\
    ../src/survival/Survival.cpp\
    ../src/sectors/Sector.cpp\
    ../src/sectors/Sectors.cpp\
    ../src/correlations/CorrelationMatrix.cpp\
    ../src/math/CholeskyDecomposition.cpp\
    ../src/math/PowMatrix.cpp\
    ../src/math/GaussianCopula.cpp\
    ../src/math/Normal.cpp\
    ../src/math/BlockMatrixChol.cpp\
    ../src/math/BlockGaussianCopula.cpp\
    ../src/portfolio/Asset.cpp\
    ../src/portfolio/DateValues.cpp\
    ../src/segmentations/Segmentations.cpp\
    ../src/segmentations/Segmentation.cpp\
    ../src/segmentations/Segment.cpp\
    ../src/utils/Exception.cpp\
    ../src/utils/Logger.cpp\
    ../src/utils/Parser.cpp\
    ../src/utils/File.cpp\
    ../src/utils/Date.cpp\
    ../src/utils/Timer.cpp\
    ../src/utils/Arrays.cpp\
    ../src/utils/Strings.cpp\
    ../src/utils/ExpatParser.cpp\
    ../src/utils/ExpatUserData.cpp\
    ../src/utils/ExpatHandlers.cpp\
    ../src/utils/Utils.cpp\
    ../src/utils/Format.cpp\
    \
    ../src/kernel/MonteCarlo.hpp\
    ../src/kernel/IData.hpp\
    ../src/kernel/SegmentAggregator.hpp\
    ../src/portfolio/Portfolio.hpp\
    ../src/portfolio/Borrower.hpp\
    ../src/params/Params.hpp\
    ../src/interests/Interests.hpp\
    ../src/interests/Interest.hpp\
    ../src/interests/Rate.hpp\
    ../src/ratings/Rating.hpp\
    ../src/ratings/Ratings.hpp\
    ../src/transitions/TransitionMatrix.hpp\
    ../src/survival/Survival.hpp\
    ../src/sectors/Sector.hpp\
    ../src/sectors/Sectors.hpp\
    ../src/correlations/CorrelationMatrix.hpp\
    ../src/math/CholeskyDecomposition.hpp\
    ../src/math/PowMatrix.hpp\
    ../src/math/Copula.hpp\
    ../src/math/GaussianCopula.hpp\
    ../src/math/Normal.hpp\
    ../src/math/BlockMatrixChol.hpp\
    ../src/math/BlockGaussianCopula.hpp\
    ../src/portfolio/Asset.hpp\
    ../src/portfolio/DateValues.hpp\
    ../src/segmentations/Segmentations.hpp\
    ../src/segmentations/Segmentation.hpp\
    ../src/segmentations/Segment.hpp\
    ../src/utils/Exception.hpp\
    ../src/utils/Logger.hpp\
    ../src/utils/Parser.hpp\
    ../src/utils/File.hpp\
    ../src/utils/Date.hpp\
    ../src/utils/Timer.hpp\
    ../src/utils/Arrays.hpp\
    ../src/utils/Strings.hpp\
    ../src/utils/ExpatParser.hpp\
    ../src/utils/ExpatUserData.hpp\
    ../src/utils/ExpatHandlers.hpp\
    ../src/utils/Utils.hpp\
    ../src/utils/Format.hpp\
    ../src/utils/ccmpi.h\
    ../src/utils/config.h

ccruncher_CXXFLAGS =
ccruncher_LDADD =
ccruncher_LDFLAGS =

# -------------------------------------------------------------
# tests
# -------------------------------------------------------------
tests_SOURCES = \
    ../share/MiniCppUnit-2.5/MiniCppUnit.cxx\
    ../share/MiniCppUnit-2.5/TestsRunner.cxx\
    ../share/gzstream-1.5/gzstream.cpp\
    ../src/utils/ArraysTest.cpp\
    ../src/utils/StringsTest.cpp\
    ../src/utils/TimerTest.cpp\
    ../src/utils/DateTest.cpp\
    ../src/utils/ParserTest.cpp\
    ../src/utils/ExceptionTest.cpp\
    ../src/utils/FileTest.cpp\
    ../src/utils/FormatTest.cpp\
    ../src/utils/UtilsTest.cpp\
    ../src/math/PowMatrixTest.cpp\
    ../src/math/NormalTest.cpp\
    ../src/math/GaussianCopulaTest.cpp\
    ../src/math/MersenneTwisterTest.cpp\
    ../src/math/CholeskyDecompositionTest.cpp\
    ../src/math/BlockMatrixCholTest.cpp\
    ../src/math/BlockGaussianCopulaTest.cpp\
    ../src/correlations/CorrelationMatrixTest.cpp\
    ../src/portfolio/AssetTest.cpp\
    ../src/portfolio/DateValuesTest.cpp\
    ../src/interests/InterestTest.cpp\
    ../src/interests/InterestsTest.cpp\
    ../src/segmentations/SegmentationTest.cpp\
    ../src/segmentations/SegmentationsTest.cpp\
    ../src/kernel/IDataTest.cpp\
    ../src/params/ParamsTest.cpp\
    ../src/portfolio/BorrowerTest.cpp\
    ../src/ratings/RatingsTest.cpp\
    ../src/sectors/SectorsTest.cpp\
    ../src/transitions/TransitionMatrixTest.cpp\
    ../src/survival/SurvivalTest.cpp\
    ../src/portfolio/Asset.cpp\
    ../src/portfolio/DateValues.cpp\
    ../src/utils/Arrays.cpp\
    ../src/utils/Strings.cpp\
    ../src/utils/Timer.cpp\
    ../src/utils/Parser.cpp\
    ../src/utils/Logger.cpp\
    ../src/utils/Date.cpp\
    ../src/utils/Exception.cpp\
    ../src/utils/File.cpp\
    ../src/utils/ExpatParser.cpp\
    ../src/utils/ExpatUserData.cpp\
    ../src/utils/ExpatHandlers.cpp\
    ../src/utils/Utils.cpp\
    ../src/utils/Format.cpp\
    ../src/math/CholeskyDecomposition.cpp\
    ../src/math/GaussianCopula.cpp\
    ../src/math/Normal.cpp\
    ../src/math/PowMatrix.cpp\
    ../src/math/BlockMatrixChol.cpp\
    ../src/math/BlockGaussianCopula.cpp\
    ../src/kernel/MonteCarlo.cpp\
    ../src/kernel/IData.cpp\
    ../src/kernel/SegmentAggregator.cpp\
    ../src/sectors/Sector.cpp\
    ../src/sectors/Sectors.cpp\
    ../src/ratings/Rating.cpp\
    ../src/ratings/Ratings.cpp\
    ../src/params/Params.cpp\
    ../src/transitions/TransitionMatrix.cpp\
    ../src/correlations/CorrelationMatrix.cpp\
    ../src/portfolio/Portfolio.cpp\
    ../src/portfolio/Borrower.cpp\
    ../src/interests/Rate.cpp\
    ../src/interests/Interest.cpp\
    ../src/interests/Interests.cpp\
    ../src/segmentations/Segmentations.cpp\
    ../src/segmentations/Segmentation.cpp\
    ../src/segmentations/Segment.cpp\
    ../src/survival/Survival.cpp\
    \
    ../src/utils/ArraysTest.hpp\
    ../src/utils/StringsTest.hpp\
    ../src/utils/TimerTest.hpp\
    ../src/utils/DateTest.hpp\
    ../src/utils/ParserTest.hpp\
    ../src/utils/ExceptionTest.hpp\
    ../src/utils/FileTest.hpp\
    ../src/utils/FormatTest.hpp\
    ../src/utils/UtilsTest.hpp\
    ../src/math/Copula.hpp\
    ../src/math/PowMatrixTest.hpp\
    ../src/math/NormalTest.hpp\
    ../src/math/GaussianCopulaTest.hpp\
    ../src/math/MersenneTwisterTest.hpp\
    ../src/math/CholeskyDecompositionTest.hpp\
    ../src/math/BlockMatrixCholTest.hpp\
    ../src/math/BlockGaussianCopulaTest.hpp\
    ../src/correlations/CorrelationMatrixTest.hpp\
    ../src/portfolio/AssetTest.hpp\
    ../src/portfolio/DateValuesTest.hpp\
    ../src/interests/InterestTest.hpp\
    ../src/interests/InterestsTest.hpp\
    ../src/segmentations/SegmentationTest.hpp\
    ../src/segmentations/SegmentationsTest.hpp\
    ../src/kernel/IDataTest.hpp\
    ../src/params/ParamsTest.hpp\
    ../src/portfolio/BorrowerTest.hpp\
    ../src/ratings/RatingsTest.hpp\
    ../src/sectors/SectorsTest.hpp\
    ../src/transitions/TransitionMatrixTest.hpp\
    ../src/survival/SurvivalTest.hpp\
    ../src/portfolio/Asset.hpp\
    ../src/portfolio/DateValues.hpp\
    ../src/utils/Arrays.hpp\
    ../src/utils/Strings.hpp\
    ../src/utils/Timer.hpp\
    ../src/utils/Parser.hpp\
    ../src/utils/Logger.hpp\
    ../src/utils/Date.hpp\
    ../src/utils/Exception.hpp\
    ../src/utils/File.hpp\
    ../src/utils/ExpatParser.hpp\
    ../src/utils/ExpatUserData.hpp\
    ../src/utils/ExpatHandlers.hpp\
    ../src/utils/Utils.hpp\
    ../src/utils/Format.hpp\
    ../src/math/CholeskyDecomposition.hpp\
    ../src/math/GaussianCopula.hpp\
    ../src/math/Normal.hpp\
    ../src/math/PowMatrix.hpp\
    ../src/math/BlockMatrixChol.hpp\
    ../src/math/BlockGaussianCopula.hpp\
    ../src/kernel/MonteCarlo.hpp\
    ../src/kernel/IData.hpp\
    ../src/kernel/SegmentAggregator.hpp\
    ../src/sectors/Sector.hpp\
    ../src/sectors/Sectors.hpp\
    ../src/ratings/Rating.hpp\
    ../src/ratings/Ratings.hpp\
    ../src/params/Params.hpp\
    ../src/transitions/TransitionMatrix.hpp\
    ../src/correlations/CorrelationMatrix.hpp\
    ../src/portfolio/Portfolio.hpp\
    ../src/portfolio/Borrower.hpp\
    ../src/interests/Rate.hpp\
    ../src/interests/Interest.hpp\
    ../src/interests/Interests.hpp\
    ../src/segmentations/Segmentations.hpp\
    ../src/segmentations/Segmentation.hpp\
    ../src/segmentations/Segment.hpp\
    ../src/survival/Survival.hpp\
    ../src/utils/ccmpi.h\
    ../src/utils/config.h

tests_CXXFLAGS =
tests_LDADD =
tests_LDFLAGS =

# -------------------------------------------------------------
# samplegen
# -------------------------------------------------------------
#samplegen_SOURCES = \
#    ../share/gzstream-1.5/gzstream.cpp\
#    ../src/kernel/samplegen.cpp\
#    ../src/kernel/MonteCarlo.cpp\
#    ../src/kernel/IData.cpp\
#    ../src/kernel/SegmentAggregator.cpp\
#    ../src/portfolio/Portfolio.cpp\
#    ../src/portfolio/Borrower.cpp\
#    ../src/params/Params.cpp\
#    ../src/interests/Interests.cpp\
#    ../src/interests/Interest.cpp\
#    ../src/interests/Rate.cpp\
#    ../src/ratings/Rating.cpp\
#    ../src/ratings/Ratings.cpp\
#    ../src/transitions/TransitionMatrix.cpp\
#    ../src/survival/Survival.cpp\
#    ../src/sectors/Sector.cpp\
#    ../src/sectors/Sectors.cpp\
#    ../src/correlations/CorrelationMatrix.cpp\
#    ../src/math/CholeskyDecomposition.cpp\
#    ../src/math/PowMatrix.cpp\
#    ../src/math/GaussianCopula.cpp\
#    ../src/math/Normal.cpp\
#    ../src/math/BlockMatrixChol.cpp\
#    ../src/math/BlockGaussianCopula.cpp\
#    ../src/portfolio/Asset.cpp\
#    ../src/portfolio/DateValues.cpp\
#    ../src/segmentations/Segmentations.cpp\
#    ../src/segmentations/Segmentation.cpp\
#    ../src/segmentations/Segment.cpp\
#    ../src/utils/Exception.cpp\
#    ../src/utils/Logger.cpp\
#    ../src/utils/Parser.cpp\
#    ../src/utils/File.cpp\
#    ../src/utils/Date.cpp\
#    ../src/utils/Timer.cpp\
#    ../src/utils/Arrays.cpp\
#    ../src/utils/Strings.cpp\
#    ../src/utils/ExpatParser.cpp\
#    ../src/utils/ExpatUserData.cpp\
#    ../src/utils/ExpatHandlers.cpp\
#    ../src/utils/Utils.cpp\
#    ../src/utils/Format.cpp\
#    \
#    ../src/kernel/MonteCarlo.hpp\
#    ../src/kernel/IData.hpp\
#    ../src/kernel/SegmentAggregator.hpp\
#    ../src/portfolio/Portfolio.hpp\
#    ../src/portfolio/Borrower.hpp\
#    ../src/params/Params.hpp\
#    ../src/interests/Interests.hpp\
#    ../src/interests/Interest.hpp\
#    ../src/interests/Rate.hpp\
#    ../src/ratings/Rating.hpp\
#    ../src/ratings/Ratings.hpp\
#    ../src/transitions/TransitionMatrix.hpp\
#    ../src/survival/Survival.hpp\
#    ../src/sectors/Sector.hpp\
#    ../src/sectors/Sectors.hpp\
#    ../src/correlations/CorrelationMatrix.hpp\
#    ../src/math/CholeskyDecomposition.hpp\
#    ../src/math/PowMatrix.hpp\
#    ../src/math/Copula.hpp\
#    ../src/math/GaussianCopula.hpp\
#    ../src/math/Normal.hpp\
#    ../src/math/BlockMatrixChol.hpp\
#    ../src/math/BlockGaussianCopula.hpp\
#    ../src/portfolio/Asset.hpp\
#    ../src/portfolio/DateValues.hpp\
#    ../src/segmentations/Segmentations.hpp\
#    ../src/segmentations/Segmentation.hpp\
#    ../src/segmentations/Segment.hpp\
#    ../src/utils/Exception.hpp\
#    ../src/utils/Logger.hpp\
#    ../src/utils/Parser.hpp\
#    ../src/utils/File.hpp\
#    ../src/utils/Date.hpp\
#    ../src/utils/Timer.hpp\
#    ../src/utils/Arrays.hpp\
#    ../src/utils/Strings.hpp\
#    ../src/utils/ExpatParser.hpp\
#    ../src/utils/ExpatUserData.hpp\
#    ../src/utils/ExpatHandlers.hpp\
#    ../src/utils/Utils.hpp\
#    ../src/utils/Format.hpp\
#    ../src/utils/ccmpi.h\
#    ../src/utils/config.h
#
#samplegen_CXXFLAGS =
#samplegen_LDADD =
#samplegen_LDFLAGS =
