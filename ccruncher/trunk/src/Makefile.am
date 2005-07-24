# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = \
    ../share/MiniCppUnit-2.2/TestsRunner.cxx\
    ../share/MiniCppUnit-2.2/MiniCppUnit.cxx\
    ../share/MiniCppUnit-2.2/MiniCppUnit.hxx\
    ../share/Mersenne-1.0/MersenneTwister.h\
    ../share/jama-1.2.4/jama_cholesky.h\
    ../share/jama-1.2.4/jama_eig.h\
    ../share/jama-1.2.4/jama_lu.h\
    ../share/jama-1.2.4/jama_qr.h\
    ../share/jama-1.2.4/jama_svd.h\
    ../share/jama-1.2.4/tnt_array1d.h\
    ../share/jama-1.2.4/tnt_array1d_utils.h\
    ../share/jama-1.2.4/tnt_array2d.h\
    ../share/jama-1.2.4/tnt_array2d_utils.h\
    ../share/jama-1.2.4/tnt_array3d.h\
    ../share/jama-1.2.4/tnt_array3d_utils.h\
    ../share/jama-1.2.4/tnt_cmat.h\
    ../share/jama-1.2.4/tnt_fortran_array1d.h\
    ../share/jama-1.2.4/tnt_fortran_array1d_utils.h\
    ../share/jama-1.2.4/tnt_fortran_array2d.h\
    ../share/jama-1.2.4/tnt_fortran_array2d_utils.h\
    ../share/jama-1.2.4/tnt_fortran_array3d.h\
    ../share/jama-1.2.4/tnt_fortran_array3d_utils.h\
    ../share/jama-1.2.4/tnt.h\
    ../share/jama-1.2.4/tnt_i_refvec.h\
    ../share/jama-1.2.4/tnt_math_utils.h\
    ../share/jama-1.2.4/tnt_sparse_matrix_csr.h\
    ../share/jama-1.2.4/tnt_stopwatch.h\
    ../share/jama-1.2.4/tnt_subscript.h\
    ../share/jama-1.2.4/tnt_vec.h\
    ../share/jama-1.2.4/tnt_version.h\
    ../share/gzstream-1.5/gzstream.h\
    ../share/gzstream-1.5/gzstream.cpp

bin_PROGRAMS = ccruncher generator mtrans

check_PROGRAMS = tests

# -------------------------------------------------------------
# ccruncher
# -------------------------------------------------------------
ccruncher_SOURCES = \
    ../share/gzstream-1.5/gzstream.cpp\
    ./kernel/ccruncher.cpp\
    ./kernel/MonteCarlo.cpp\
    ./kernel/IData.cpp\
    ./kernel/SegmentAggregator.cpp\
    ./portfolio/Portfolio.cpp\
    ./portfolio/Client.cpp\
    ./params/Params.cpp\
    ./interests/Interests.cpp\
    ./interests/Interest.cpp\
    ./interests/Rate.cpp\
    ./ratings/Rating.cpp\
    ./ratings/Ratings.cpp\
    ./transitions/TransitionMatrix.cpp\
    ./survival/Survival.cpp\
    ./sectors/Sector.cpp\
    ./sectors/Sectors.cpp\
    ./correlations/CorrelationMatrix.cpp\
    ./math/CholeskyDecomposition.cpp\
    ./math/PowMatrix.cpp\
    ./math/GaussianCopula.cpp\
    ./math/Normal.cpp\
    ./math/BlockMatrixChol.cpp\
    ./math/BlockGaussianCopula.cpp\
    ./portfolio/Asset.cpp\
    ./portfolio/DateValues.cpp\
    ./segmentations/Segmentations.cpp\
    ./segmentations/Segmentation.cpp\
    ./segmentations/Segment.cpp\
    ./utils/Exception.cpp\
    ./utils/Logger.cpp\
    ./utils/Parser.cpp\
    ./utils/File.cpp\
    ./utils/Date.cpp\
    ./utils/Timer.cpp\
    ./utils/Arrays.cpp\
    ./utils/Strings.cpp\
    ./utils/ExpatParser.cpp\
    ./utils/ExpatUserData.cpp\
    ./utils/ExpatHandlers.cpp\
    ./utils/Utils.cpp\
    ./utils/Format.cpp\
    \
    ./kernel/MonteCarlo.hpp\
    ./kernel/IData.hpp\
    ./kernel/SegmentAggregator.hpp\
    ./portfolio/Portfolio.hpp\
    ./portfolio/Client.hpp\
    ./params/Params.hpp\
    ./interests/Interests.hpp\
    ./interests/Interest.hpp\
    ./interests/Rate.hpp\
    ./ratings/Rating.hpp\
    ./ratings/Ratings.hpp\
    ./transitions/TransitionMatrix.hpp\
    ./survival/Survival.hpp\
    ./sectors/Sector.hpp\
    ./sectors/Sectors.hpp\
    ./correlations/CorrelationMatrix.hpp\
    ./math/CholeskyDecomposition.hpp\
    ./math/PowMatrix.hpp\
    ./math/Copula.hpp\
    ./math/GaussianCopula.hpp\
    ./math/Normal.hpp\
    ./math/BlockMatrixChol.hpp\
    ./math/BlockGaussianCopula.hpp\
    ./portfolio/Asset.hpp\
    ./portfolio/DateValues.hpp\
    ./segmentations/Segmentations.hpp\
    ./segmentations/Segmentation.hpp\
    ./segmentations/Segment.hpp\
    ./utils/Exception.hpp\
    ./utils/Logger.hpp\
    ./utils/Parser.hpp\
    ./utils/File.hpp\
    ./utils/Date.hpp\
    ./utils/Timer.hpp\
    ./utils/Arrays.hpp\
    ./utils/Strings.hpp\
    ./utils/ExpatParser.hpp\
    ./utils/ExpatUserData.hpp\
    ./utils/ExpatHandlers.hpp\
    ./utils/Utils.hpp\
    ./utils/Format.hpp

ccruncher_CXXFLAGS = -Wall -W -Wshadow
ccruncher_LDADD = -lm -lexpat -lz
ccruncher_LDFLAGS = -rdynamic

# -------------------------------------------------------------
# generator
# -------------------------------------------------------------
generator_SOURCES = \
    ../share/gzstream-1.5/gzstream.cpp\
    ./tools/generator.cpp\
    ./tools/Bond.cpp\
    ./kernel/IData.cpp\
    ./portfolio/Portfolio.cpp\
    ./portfolio/Client.cpp\
    ./params/Params.cpp\
    ./interests/Interests.cpp\
    ./interests/Interest.cpp\
    ./interests/Rate.cpp\
    ./ratings/Rating.cpp\
    ./ratings/Ratings.cpp\
    ./transitions/TransitionMatrix.cpp\
    ./survival/Survival.cpp\
    ./sectors/Sector.cpp\
    ./sectors/Sectors.cpp\
    ./correlations/CorrelationMatrix.cpp\
    ./math/CholeskyDecomposition.cpp\
    ./math/PowMatrix.cpp\
    ./math/GaussianCopula.cpp\
    ./math/Normal.cpp\
    ./math/BlockMatrixChol.cpp\
    ./math/BlockGaussianCopula.cpp\
    ./portfolio/Asset.cpp\
    ./portfolio/DateValues.cpp\
    ./segmentations/Segmentations.cpp\
    ./segmentations/Segmentation.cpp\
    ./segmentations/Segment.cpp\
    ./utils/Exception.cpp\
    ./utils/Parser.cpp\
    ./utils/Logger.cpp\
    ./utils/File.cpp\
    ./utils/Date.cpp\
    ./utils/Timer.cpp\
    ./utils/Arrays.cpp\
    ./utils/Strings.cpp\
    ./utils/ExpatParser.cpp\
    ./utils/ExpatUserData.cpp\
    ./utils/ExpatHandlers.cpp\
    ./utils/Utils.cpp\
    ./utils/Format.cpp\
    \
    ./tools/Bond.hpp\
    ./kernel/IData.hpp\
    ./portfolio/Portfolio.hpp\
    ./portfolio/Client.hpp\
    ./params/Params.hpp\
    ./interests/Interests.hpp\
    ./interests/Interest.hpp\
    ./interests/Rate.hpp\
    ./ratings/Rating.hpp\
    ./ratings/Ratings.hpp\
    ./transitions/TransitionMatrix.hpp\
    ./survival/Survival.hpp\
    ./sectors/Sector.hpp\
    ./sectors/Sectors.hpp\
    ./correlations/CorrelationMatrix.hpp\
    ./math/CholeskyDecomposition.hpp\
    ./math/Copula.hpp\
    ./math/PowMatrix.hpp\
    ./math/GaussianCopula.hpp\
    ./math/Normal.hpp\
    ./math/BlockMatrixChol.hpp\
    ./math/BlockGaussianCopula.hpp\
    ./portfolio/Asset.hpp\
    ./portfolio/DateValues.hpp\
    ./segmentations/Segmentations.hpp\
    ./segmentations/Segmentation.hpp\
    ./segmentations/Segment.hpp\
    ./utils/Exception.hpp\
    ./utils/Parser.hpp\
    ./utils/Logger.hpp\
    ./utils/File.hpp\
    ./utils/Date.hpp\
    ./utils/Timer.hpp\
    ./utils/Arrays.hpp\
    ./utils/Strings.hpp\
    ./utils/ExpatParser.hpp\
    ./utils/ExpatUserData.hpp\
    ./utils/ExpatHandlers.hpp\
    ./utils/Utils.hpp\
    ./utils/Format.hpp

generator_CXXFLAGS = -Wall -W -Wshadow
generator_LDADD = -lm -lexpat -lz
generator_LDFLAGS = -rdynamic

# -------------------------------------------------------------
# mtrans
# -------------------------------------------------------------
mtrans_SOURCES = \
    ../share/gzstream-1.5/gzstream.cpp\
    ./tools/mtrans.cpp\
    ./kernel/IData.cpp\
    ./portfolio/Portfolio.cpp\
    ./portfolio/Client.cpp\
    ./params/Params.cpp\
    ./interests/Interests.cpp\
    ./interests/Interest.cpp\
    ./interests/Rate.cpp\
    ./ratings/Rating.cpp\
    ./ratings/Ratings.cpp\
    ./transitions/TransitionMatrix.cpp\
    ./survival/Survival.cpp\
    ./sectors/Sector.cpp\
    ./sectors/Sectors.cpp\
    ./correlations/CorrelationMatrix.cpp\
    ./math/CholeskyDecomposition.cpp\
    ./math/PowMatrix.cpp\
    ./math/GaussianCopula.cpp\
    ./math/Normal.cpp\
    ./math/BlockMatrixChol.cpp\
    ./math/BlockGaussianCopula.cpp\
    ./portfolio/Asset.cpp\
    ./portfolio/DateValues.cpp\
    ./segmentations/Segmentations.cpp\
    ./segmentations/Segmentation.cpp\
    ./segmentations/Segment.cpp\
    ./utils/Exception.cpp\
    ./utils/Parser.cpp\
    ./utils/Logger.cpp\
    ./utils/File.cpp\
    ./utils/Date.cpp\
    ./utils/Timer.cpp\
    ./utils/Arrays.cpp\
    ./utils/Strings.cpp\
    ./utils/ExpatParser.cpp\
    ./utils/ExpatUserData.cpp\
    ./utils/ExpatHandlers.cpp\
    ./utils/Utils.cpp\
    ./utils/Format.cpp\
    \
    ./kernel/IData.hpp\
    ./portfolio/Portfolio.hpp\
    ./portfolio/Client.hpp\
    ./params/Params.hpp\
    ./interests/Interests.hpp\
    ./interests/Interest.hpp\
    ./interests/Rate.hpp\
    ./ratings/Rating.hpp\
    ./ratings/Ratings.hpp\
    ./transitions/TransitionMatrix.hpp\
    ./survival/Survival.hpp\
    ./sectors/Sector.hpp\
    ./sectors/Sectors.hpp\
    ./correlations/CorrelationMatrix.hpp\
    ./math/CholeskyDecomposition.hpp\
    ./math/Copula.hpp\
    ./math/PowMatrix.hpp\
    ./math/GaussianCopula.hpp\
    ./math/Normal.hpp\
    ./math/BlockMatrixChol.hpp\
    ./math/BlockGaussianCopula.hpp\
    ./portfolio/Asset.hpp\
    ./portfolio/DateValues.hpp\
    ./segmentations/Segmentations.hpp\
    ./segmentations/Segmentation.hpp\
    ./segmentations/Segment.hpp\
    ./utils/Exception.hpp\
    ./utils/Parser.hpp\
    ./utils/Logger.hpp\
    ./utils/File.hpp\
    ./utils/Date.hpp\
    ./utils/Timer.hpp\
    ./utils/Arrays.hpp\
    ./utils/Strings.hpp\
    ./utils/ExpatParser.hpp\
    ./utils/ExpatUserData.hpp\
    ./utils/ExpatHandlers.hpp\
    ./utils/Utils.hpp\
    ./utils/Format.hpp

mtrans_CXXFLAGS = -Wall -W -Wshadow
mtrans_LDADD = -lm -lexpat -lz
mtrans_LDFLAGS = -rdynamic

# -------------------------------------------------------------
# tests
# -------------------------------------------------------------
tests_SOURCES = \
    ../share/MiniCppUnit-2.2/MiniCppUnit.cxx\
    ../share/MiniCppUnit-2.2/TestsRunner.cxx\
    ../share/gzstream-1.5/gzstream.cpp\
    ./utils/ArraysTest.cpp\
    ./utils/StringsTest.cpp\
    ./utils/TimerTest.cpp\
    ./utils/DateTest.cpp\
    ./utils/ParserTest.cpp\
    ./utils/ExceptionTest.cpp\
    ./utils/FileTest.cpp\
    ./utils/FormatTest.cpp\
    ./math/PowMatrixTest.cpp\
    ./math/NormalTest.cpp\
    ./math/GaussianCopulaTest.cpp\
    ./math/MersenneTwisterTest.cpp\
    ./math/CholeskyDecompositionTest.cpp\
    ./math/BlockMatrixCholTest.cpp\
    ./math/BlockGaussianCopulaTest.cpp\
    ./correlations/CorrelationMatrixTest.cpp\
    ./portfolio/AssetTest.cpp\
    ./portfolio/DateValuesTest.cpp\
    ./interests/InterestTest.cpp\
    ./interests/InterestsTest.cpp\
    ./segmentations/SegmentationTest.cpp\
    ./segmentations/SegmentationsTest.cpp\
    ./kernel/IDataTest.cpp\
    ./params/ParamsTest.cpp\
    ./portfolio/ClientTest.cpp\
    ./ratings/RatingsTest.cpp\
    ./sectors/SectorsTest.cpp\
    ./transitions/TransitionMatrixTest.cpp\
    ./survival/SurvivalTest.cpp\
    ./portfolio/Asset.cpp\
    ./portfolio/DateValues.cpp\
    ./utils/Arrays.cpp\
    ./utils/Strings.cpp\
    ./utils/Timer.cpp\
    ./utils/Parser.cpp\
    ./utils/Logger.cpp\
    ./utils/Date.cpp\
    ./utils/Exception.cpp\
    ./utils/File.cpp\
    ./utils/ExpatParser.cpp\
    ./utils/ExpatUserData.cpp\
    ./utils/ExpatHandlers.cpp\
    ./utils/Utils.cpp\
    ./utils/Format.cpp\
    ./math/CholeskyDecomposition.cpp\
    ./math/GaussianCopula.cpp\
    ./math/Normal.cpp\
    ./math/PowMatrix.cpp\
    ./math/BlockMatrixChol.cpp\
    ./math/BlockGaussianCopula.cpp\
    ./kernel/MonteCarlo.cpp\
    ./kernel/IData.cpp\
    ./kernel/SegmentAggregator.cpp\
    ./sectors/Sector.cpp\
    ./sectors/Sectors.cpp\
    ./ratings/Rating.cpp\
    ./ratings/Ratings.cpp\
    ./params/Params.cpp\
    ./transitions/TransitionMatrix.cpp\
    ./correlations/CorrelationMatrix.cpp\
    ./portfolio/Portfolio.cpp\
    ./portfolio/Client.cpp\
    ./interests/Rate.cpp\
    ./interests/Interest.cpp\
    ./interests/Interests.cpp\
    ./segmentations/Segmentations.cpp\
    ./segmentations/Segmentation.cpp\
    ./segmentations/Segment.cpp\
    ./survival/Survival.cpp\
    \
    ./utils/ArraysTest.hpp\
    ./utils/StringsTest.hpp\
    ./utils/TimerTest.hpp\
    ./utils/DateTest.hpp\
    ./utils/ParserTest.hpp\
    ./utils/ExceptionTest.hpp\
    ./utils/FileTest.hpp\
    ./utils/FormatTest.hpp\
    ./math/Copula.hpp\
    ./math/PowMatrixTest.hpp\
    ./math/NormalTest.hpp\
    ./math/GaussianCopulaTest.hpp\
    ./math/MersenneTwisterTest.hpp\
    ./math/CholeskyDecompositionTest.hpp\
    ./math/BlockMatrixCholTest.hpp\
    ./math/BlockGaussianCopulaTest.hpp\
    ./correlations/CorrelationMatrixTest.hpp\
    ./portfolio/AssetTest.hpp\
    ./portfolio/DateValuesTest.hpp\
    ./interests/InterestTest.hpp\
    ./interests/InterestsTest.hpp\
    ./segmentations/SegmentationTest.hpp\
    ./segmentations/SegmentationsTest.hpp\
    ./kernel/IDataTest.hpp\
    ./params/ParamsTest.hpp\
    ./portfolio/ClientTest.hpp\
    ./ratings/RatingsTest.hpp\
    ./sectors/SectorsTest.hpp\
    ./transitions/TransitionMatrixTest.hpp\
    ./survival/SurvivalTest.hpp\
    ./portfolio/Asset.hpp\
    ./portfolio/DateValues.hpp\
    ./utils/Arrays.hpp\
    ./utils/Strings.hpp\
    ./utils/Timer.hpp\
    ./utils/Parser.hpp\
    ./utils/Logger.hpp\
    ./utils/Date.hpp\
    ./utils/Exception.hpp\
    ./utils/File.hpp\
    ./utils/ExpatParser.hpp\
    ./utils/ExpatUserData.hpp\
    ./utils/ExpatHandlers.hpp\
    ./utils/Utils.hpp\
    ./utils/Format.hpp\
    ./math/CholeskyDecomposition.hpp\
    ./math/GaussianCopula.hpp\
    ./math/Normal.hpp\
    ./math/PowMatrix.hpp\
    ./math/BlockMatrixChol.hpp\
    ./math/BlockGaussianCopula.hpp\
    ./kernel/MonteCarlo.hpp\
    ./kernel/IData.hpp\
    ./kernel/SegmentAggregator.hpp\
    ./sectors/Sector.hpp\
    ./sectors/Sectors.hpp\
    ./ratings/Rating.hpp\
    ./ratings/Ratings.hpp\
    ./params/Params.hpp\
    ./transitions/TransitionMatrix.hpp\
    ./correlations/CorrelationMatrix.hpp\
    ./portfolio/Portfolio.hpp\
    ./portfolio/Client.hpp\
    ./interests/Rate.hpp\
    ./interests/Interest.hpp\
    ./interests/Interests.hpp\
    ./segmentations/Segmentations.hpp\
    ./segmentations/Segmentation.hpp\
    ./segmentations/Segment.hpp\
    ./survival/Survival.hpp

tests_CXXFLAGS = -Wall -W -Wshadow
tests_LDADD = -lm -lexpat -lz
tests_LDFLAGS = -rdynamic
