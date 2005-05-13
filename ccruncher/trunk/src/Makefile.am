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
    ../share/jama-1.2.2/jama_cholesky.h\
    ../share/jama-1.2.2/jama_eig.h\
    ../share/jama-1.2.2/jama_lu.h\
    ../share/jama-1.2.2/jama_qr.h\
    ../share/jama-1.2.2/jama_svd.h\
    ../share/jama-1.2.2/tnt_array1d.h\
    ../share/jama-1.2.2/tnt_array1d_utils.h\
    ../share/jama-1.2.2/tnt_array2d.h\
    ../share/jama-1.2.2/tnt_array2d_utils.h\
    ../share/jama-1.2.2/tnt_array3d.h\
    ../share/jama-1.2.2/tnt_array3d_utils.h\
    ../share/jama-1.2.2/tnt_cmat.h\
    ../share/jama-1.2.2/tnt_fortran_array1d.h\
    ../share/jama-1.2.2/tnt_fortran_array1d_utils.h\
    ../share/jama-1.2.2/tnt_fortran_array2d.h\
    ../share/jama-1.2.2/tnt_fortran_array2d_utils.h\
    ../share/jama-1.2.2/tnt_fortran_array3d.h\
    ../share/jama-1.2.2/tnt_fortran_array3d_utils.h\
    ../share/jama-1.2.2/tnt.h\
    ../share/jama-1.2.2/tnt_i_refvec.h\
    ../share/jama-1.2.2/tnt_math_utils.h\
    ../share/jama-1.2.2/tnt_sparse_matrix_csr.h\
    ../share/jama-1.2.2/tnt_stopwatch.h\
    ../share/jama-1.2.2/tnt_subscript.h\
    ../share/jama-1.2.2/tnt_vec.h\
    ../share/jama-1.2.2/tnt_version.h

bin_PROGRAMS = ccruncher cc2gpi generator mtrans

check_PROGRAMS = tests

# -------------------------------------------------------------
# ccruncher
# -------------------------------------------------------------
ccruncher_SOURCES = \
    ./kernel/ccruncher.cpp\
    ./kernel/MonteCarlo.cpp\
    ./kernel/IData.cpp\
    ./portfolio/Portfolio.cpp\
    ./portfolio/Client.cpp\
    ./params/Params.cpp\
    ./interests/Interests.cpp\
    ./interests/Interest.cpp\
    ./interests/Rate.cpp\
    ./ratings/Rating.cpp\
    ./ratings/Ratings.cpp\
    ./ratings/DateRatings.cpp\
    ./transitions/TransitionMatrix.cpp\
    ./sectors/Sector.cpp\
    ./sectors/Sectors.cpp\
    ./correlations/CorrelationMatrix.cpp\
    ./math/CholeskyDecomposition.cpp\
    ./math/PowMatrix.cpp\
    ./math/CopulaNormal.cpp\
    ./math/Normal.cpp\
    ./portfolio/Asset.cpp\
    ./portfolio/DateValues.cpp\
    ./segmentations/Segmentations.cpp\
    ./segmentations/Segmentation.cpp\
    ./segmentations/Segment.cpp\
    ./aggregators/SegmentAggregator.cpp\
    ./aggregators/Aggregators.cpp\
    ./aggregators/Aggregator.cpp\
    ./utils/Exception.cpp\
    ./utils/Logger.cpp\
    ./utils/Parser.cpp\
    ./utils/File.cpp\
    ./utils/Date.cpp\
    ./utils/Timer.cpp\
    ./utils/Utils.cpp\
    ./utils/ExpatParser.cpp\
    ./utils/ExpatUserData.cpp\
    ./utils/ExpatHandlers.cpp\
    \
    ./kernel/MonteCarlo.hpp\
    ./kernel/IData.hpp\
    ./portfolio/Portfolio.hpp\
    ./portfolio/Client.hpp\
    ./params/Params.hpp\
    ./interests/Interests.hpp\
    ./interests/Interest.hpp\
    ./interests/Rate.hpp\
    ./ratings/Rating.hpp\
    ./ratings/Ratings.hpp\
    ./ratings/DateRatings.hpp\
    ./transitions/TransitionMatrix.hpp\
    ./sectors/Sector.hpp\
    ./sectors/Sectors.hpp\
    ./correlations/CorrelationMatrix.hpp\
    ./math/CholeskyDecomposition.hpp\
    ./math/PowMatrix.hpp\
    ./math/Copula.hpp\
    ./math/CopulaNormal.hpp\
    ./math/Normal.hpp\
    ./portfolio/Asset.hpp\
    ./portfolio/DateValues.hpp\
    ./segmentations/Segmentations.hpp\
    ./segmentations/Segmentation.hpp\
    ./segmentations/Segment.hpp\
    ./aggregators/SegmentAggregator.hpp\
    ./aggregators/Aggregators.hpp\
    ./aggregators/Aggregator.hpp\
    ./utils/Exception.hpp\
    ./utils/Logger.hpp\
    ./utils/Parser.hpp\
    ./utils/File.hpp\
    ./utils/Date.hpp\
    ./utils/Timer.hpp\
    ./utils/Utils.hpp\
    ./utils/ExpatParser.hpp\
    ./utils/ExpatUserData.hpp\
    ./utils/ExpatHandlers.hpp

ccruncher_CXXFLAGS = -Wall -W -Wshadow
ccruncher_LDADD = -lm -lexpat

# -------------------------------------------------------------
# cc2gpi
# -------------------------------------------------------------
cc2gpi_SOURCES = \
    ./tools/cc2gpi.cpp\
    ./tools/FileResults.cpp\
    ./utils/Exception.cpp\
    ./utils/Parser.cpp\
    ./utils/Date.cpp\
    ./utils/Utils.cpp\
    ./utils/File.cpp\
    \
    ./tools/FileResults.hpp\
    ./utils/Exception.hpp\
    ./utils/Parser.hpp\
    ./utils/Date.hpp\
    ./utils/Utils.hpp\
    ./utils/File.hpp

cc2gpi_CXXFLAGS = -Wall -W -Wshadow
cc2gpi_LDADD = 

# -------------------------------------------------------------
# generator
# -------------------------------------------------------------
generator_SOURCES = \
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
    ./ratings/DateRatings.cpp\
    ./transitions/TransitionMatrix.cpp\
    ./sectors/Sector.cpp\
    ./sectors/Sectors.cpp\
    ./correlations/CorrelationMatrix.cpp\
    ./math/CholeskyDecomposition.cpp\
    ./math/PowMatrix.cpp\
    ./math/CopulaNormal.cpp\
    ./math/Normal.cpp\
    ./portfolio/Asset.cpp\
    ./portfolio/DateValues.cpp\
    ./segmentations/Segmentations.cpp\
    ./segmentations/Segmentation.cpp\
    ./segmentations/Segment.cpp\
    ./aggregators/SegmentAggregator.cpp\
    ./aggregators/Aggregators.cpp\
    ./aggregators/Aggregator.cpp\
    ./utils/Exception.cpp\
    ./utils/Parser.cpp\
    ./utils/Logger.cpp\
    ./utils/File.cpp\
    ./utils/Date.cpp\
    ./utils/Timer.cpp\
    ./utils/Utils.cpp\
    ./utils/ExpatParser.cpp\
    ./utils/ExpatUserData.cpp\
    ./utils/ExpatHandlers.cpp\
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
    ./ratings/DateRatings.hpp\
    ./transitions/TransitionMatrix.hpp\
    ./sectors/Sector.hpp\
    ./sectors/Sectors.hpp\
    ./correlations/CorrelationMatrix.hpp\
    ./math/CholeskyDecomposition.hpp\
    ./math/Copula.hpp\
    ./math/PowMatrix.hpp\
    ./math/CopulaNormal.hpp\
    ./math/Normal.hpp\
    ./portfolio/Asset.hpp\
    ./portfolio/DateValues.hpp\
    ./segmentations/Segmentations.hpp\
    ./segmentations/Segmentation.hpp\
    ./segmentations/Segment.hpp\
    ./aggregators/SegmentAggregator.hpp\
    ./aggregators/Aggregators.hpp\
    ./aggregators/Aggregator.hpp\
    ./utils/Exception.hpp\
    ./utils/Parser.hpp\
    ./utils/Logger.hpp\
    ./utils/File.hpp\
    ./utils/Date.hpp\
    ./utils/Timer.hpp\
    ./utils/Utils.hpp\
    ./utils/ExpatParser.hpp\
    ./utils/ExpatUserData.hpp\
    ./utils/ExpatHandlers.hpp

generator_CXXFLAGS = -Wall -W -Wshadow
generator_LDADD = -lm -lexpat

# -------------------------------------------------------------
# mtrans
# -------------------------------------------------------------
mtrans_SOURCES = \
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
    ./ratings/DateRatings.cpp\
    ./transitions/TransitionMatrix.cpp\
    ./sectors/Sector.cpp\
    ./sectors/Sectors.cpp\
    ./correlations/CorrelationMatrix.cpp\
    ./math/CholeskyDecomposition.cpp\
    ./math/PowMatrix.cpp\
    ./math/CopulaNormal.cpp\
    ./math/Normal.cpp\
    ./portfolio/Asset.cpp\
    ./portfolio/DateValues.cpp\
    ./segmentations/Segmentations.cpp\
    ./segmentations/Segmentation.cpp\
    ./segmentations/Segment.cpp\
    ./aggregators/SegmentAggregator.cpp\
    ./aggregators/Aggregators.cpp\
    ./aggregators/Aggregator.cpp\
    ./utils/Exception.cpp\
    ./utils/Parser.cpp\
    ./utils/Logger.cpp\
    ./utils/File.cpp\
    ./utils/Date.cpp\
    ./utils/Timer.cpp\
    ./utils/Utils.cpp\
    ./utils/ExpatParser.cpp\
    ./utils/ExpatUserData.cpp\
    ./utils/ExpatHandlers.cpp\
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
    ./ratings/DateRatings.hpp\
    ./transitions/TransitionMatrix.hpp\
    ./sectors/Sector.hpp\
    ./sectors/Sectors.hpp\
    ./correlations/CorrelationMatrix.hpp\
    ./math/CholeskyDecomposition.hpp\
    ./math/Copula.hpp\
    ./math/PowMatrix.hpp\
    ./math/CopulaNormal.hpp\
    ./math/Normal.hpp\
    ./portfolio/Asset.hpp\
    ./portfolio/DateValues.hpp\
    ./segmentations/Segmentations.hpp\
    ./segmentations/Segmentation.hpp\
    ./segmentations/Segment.hpp\
    ./aggregators/SegmentAggregator.hpp\
    ./aggregators/Aggregators.hpp\
    ./aggregators/Aggregator.hpp\
    ./utils/Exception.hpp\
    ./utils/Parser.hpp\
    ./utils/Logger.hpp\
    ./utils/File.hpp\
    ./utils/Date.hpp\
    ./utils/Timer.hpp\
    ./utils/Utils.hpp\
    ./utils/ExpatParser.hpp\
    ./utils/ExpatUserData.hpp\
    ./utils/ExpatHandlers.hpp

mtrans_CXXFLAGS = -Wall -W -Wshadow
mtrans_LDADD = -lm -lexpat

# -------------------------------------------------------------
# tests
# -------------------------------------------------------------
tests_SOURCES = \
    ../share/MiniCppUnit-2.2/MiniCppUnit.cxx\
    ../share/MiniCppUnit-2.2/TestsRunner.cxx\
    ./utils/UtilsTest.cpp\
    ./utils/TimerTest.cpp\
    ./utils/DateTest.cpp\
    ./utils/ParserTest.cpp\
    ./utils/ExceptionTest.cpp\
    ./utils/FileTest.cpp\
    ./math/PowMatrixTest.cpp\
    ./math/NormalTest.cpp\
    ./math/CopulaNormalTest.cpp\
    ./math/MersenneTwisterTest.cpp\
    ./math/CholeskyDecompositionTest.cpp\
    ./aggregators/AggregatorsTest.cpp\
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
    ./portfolio/Asset.cpp\
    ./portfolio/DateValues.cpp\
    ./utils/Timer.cpp\
    ./utils/Parser.cpp\
    ./utils/Utils.cpp\
    ./utils/Logger.cpp\
    ./utils/Date.cpp\
    ./utils/Exception.cpp\
    ./utils/File.cpp\
    ./utils/ExpatParser.cpp\
    ./utils/ExpatUserData.cpp\
    ./utils/ExpatHandlers.cpp\
    ./math/CholeskyDecomposition.cpp\
    ./math/CopulaNormal.cpp\
    ./math/Normal.cpp\
    ./math/PowMatrix.cpp\
    ./kernel/MonteCarlo.cpp\
    ./kernel/IData.cpp\
    ./sectors/Sector.cpp\
    ./sectors/Sectors.cpp\
    ./ratings/Rating.cpp\
    ./ratings/Ratings.cpp\
    ./ratings/DateRatings.cpp\
    ./params/Params.cpp\
    ./transitions/TransitionMatrix.cpp\
    ./correlations/CorrelationMatrix.cpp\
    ./portfolio/Portfolio.cpp\
    ./portfolio/Client.cpp\
    ./interests/Rate.cpp\
    ./interests/Interest.cpp\
    ./interests/Interests.cpp\
    ./aggregators/Aggregator.cpp\
    ./aggregators/Aggregators.cpp\
    ./aggregators/SegmentAggregator.cpp\
    ./segmentations/Segmentations.cpp\
    ./segmentations/Segmentation.cpp\
    ./segmentations/Segment.cpp\
    \
    ./utils/UtilsTest.hpp\
    ./utils/TimerTest.hpp\
    ./utils/DateTest.hpp\
    ./utils/ParserTest.hpp\
    ./utils/ExceptionTest.hpp\
    ./utils/FileTest.hpp\
    ./math/Copula.hpp\
    ./math/PowMatrixTest.hpp\
    ./math/NormalTest.hpp\
    ./math/CopulaNormalTest.hpp\
    ./math/MersenneTwisterTest.hpp\
    ./math/CholeskyDecompositionTest.hpp\
    ./aggregators/AggregatorsTest.hpp\
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
    ./portfolio/Asset.hpp\
    ./portfolio/DateValues.hpp\
    ./utils/Timer.hpp\
    ./utils/Parser.hpp\
    ./utils/Utils.hpp\
    ./utils/Logger.hpp\
    ./utils/Date.hpp\
    ./utils/Exception.hpp\
    ./utils/File.hpp\
    ./utils/ExpatParser.hpp\
    ./utils/ExpatUserData.hpp\
    ./utils/ExpatHandlers.hpp\
    ./math/CholeskyDecomposition.hpp\
    ./math/CopulaNormal.hpp\
    ./math/Normal.hpp\
    ./math/PowMatrix.hpp\
    ./kernel/MonteCarlo.hpp\
    ./kernel/IData.hpp\
    ./sectors/Sector.hpp\
    ./sectors/Sectors.hpp\
    ./ratings/Rating.hpp\
    ./ratings/Ratings.hpp\
    ./ratings/DateRatings.hpp\
    ./params/Params.hpp\
    ./transitions/TransitionMatrix.hpp\
    ./correlations/CorrelationMatrix.hpp\
    ./portfolio/Portfolio.hpp\
    ./portfolio/Client.hpp\
    ./interests/Rate.hpp\
    ./interests/Interest.hpp\
    ./interests/Interests.hpp\
    ./aggregators/Aggregator.hpp\
    ./aggregators/Aggregators.hpp\
    ./aggregators/SegmentAggregator.hpp\
    ./segmentations/Segmentations.hpp\
    ./segmentations/Segmentation.hpp\
    ./segmentations/Segment.hpp

tests_CXXFLAGS = -Wall -W -Wshadow
tests_LDADD = -lm -lexpat
