# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = \
    src bin data doc samples \
    ccruncher-cmd.pro \
    ccruncher-gui.pro \
    ccruncher-tests.pro \
    ccruncher-inf.pro \
    Doxyfile

bin_PROGRAMS = \
    build/ccruncher-cmd \
    build/ccruncher-inf

check_PROGRAMS = build/ccruncher-tests

TESTS = build/ccruncher-tests

man1_MANS = \
    doc/ccruncher-cmd.1 \
    doc/ccruncher-inf.1

dist_doc_DATA = \
    doc/AUTHORS \
    doc/ChangeLog \
    doc/COPYING \
    doc/README \
    doc/TODO \
    doc/ccruncher.pdf \
    doc/index.html

# -------------------------------------------------------------
# install/unistall samples and html directories in docdir
# -------------------------------------------------------------
install-data-hook:
	cp -rvf $(srcdir)/doc/html $(DESTDIR)$(docdir);
	cp -rvf $(srcdir)/samples $(DESTDIR)$(docdir);

uninstall-hook:
	chmod -R +w $(DESTDIR)$(docdir)/html && rm -rvf $(DESTDIR)$(docdir)/html;
	chmod -R +w $(DESTDIR)$(docdir)/samples && rm -rvf $(DESTDIR)$(docdir)/samples;

# -------------------------------------------------------------
# removes subversion stuff
# -------------------------------------------------------------
dist-hook:
	rm -rf `find $(distdir) -name \.svn`

# -------------------------------------------------------------
# ccruncher-cmd
# -------------------------------------------------------------
build_ccruncher_cmd_SOURCES = \
    src/ccruncher-cmd.cpp \
    src/kernel/MonteCarlo.cpp \
    src/kernel/Input.cpp \
    src/kernel/InputData.cpp \
    src/kernel/XmlInputData.cpp \
    src/kernel/Aggregator.cpp \
    src/kernel/Inverse.cpp \
    src/kernel/SimulationThread.cpp \
    src/params/Params.cpp \
    src/params/Interest.cpp \
    src/params/Rating.cpp \
    src/params/Transitions.cpp \
    src/params/Factor.cpp \
    src/params/Segmentation.cpp \
    src/params/CDF.cpp \
    src/portfolio/Obligor.cpp \
    src/portfolio/Asset.cpp \
    src/portfolio/DateValues.cpp \
    src/portfolio/LGD.cpp \
    src/portfolio/EAD.cpp \
    src/utils/PowMatrix.cpp \
    src/utils/Exception.cpp \
    src/utils/Logger.cpp \
    src/utils/Parser.cpp \
    src/utils/Date.cpp \
    src/utils/Expr.cpp \
    src/utils/ExpatParser.cpp \
    src/utils/ExpatUserData.cpp \
    src/utils/ExpatHandlers.cpp \
    src/utils/Utils.cpp \
    src/utils/Thread.cpp \
    \
    src/kernel/MonteCarlo.hpp \
    src/kernel/Input.hpp \
    src/kernel/InputData.hpp \
    src/kernel/XmlInputData.hpp \
    src/kernel/Aggregator.hpp \
    src/kernel/Inverse.hpp \
    src/kernel/SimulationThread.hpp \
    src/params/Params.hpp \
    src/params/Interest.hpp \
    src/params/Rating.hpp \
    src/params/Transitions.hpp \
    src/params/Factor.hpp \
    src/params/Segmentation.hpp \
    src/params/CDF.hpp \
    src/portfolio/Obligor.hpp \
    src/portfolio/Asset.hpp \
    src/portfolio/DateValues.hpp \
    src/portfolio/LGD.hpp \
    src/portfolio/EAD.hpp \
    src/utils/PowMatrix.hpp \
    src/utils/Exception.hpp \
    src/utils/Logger.hpp \
    src/utils/Parser.hpp \
    src/utils/Date.hpp \
    src/utils/Expr.hpp \
    src/utils/ExpatParser.hpp \
    src/utils/ExpatUserData.hpp \
    src/utils/ExpatHandlers.hpp \
    src/utils/Utils.hpp \
    src/utils/Thread.hpp \
    src/utils/config.h

#build_ccruncher_cmd_CXXFLAGS =
#build_ccruncher_cmd_LDADD =
#build_ccruncher_cmd_LDFLAGS =

# -------------------------------------------------------------
# ccruncher-tests
# -------------------------------------------------------------
build_ccruncher_tests_SOURCES = \
    src/ccruncher-tests.cpp \
    src/utils/MiniCppUnit.cxx\
    src/utils/DateTest.cpp \
    src/utils/ExprTest.cpp \
    src/utils/ParserTest.cpp \
    src/utils/ExceptionTest.cpp \
    src/utils/UtilsTest.cpp \
    src/utils/ExpatUserDataTest.cpp \
    src/utils/PowMatrixTest.cpp \
    src/portfolio/AssetTest.cpp \
    src/portfolio/DateValuesTest.cpp \
    src/portfolio/LGDTest.cpp \
    src/portfolio/EADTest.cpp \
    src/portfolio/ObligorTest.cpp \
    src/params/InterestTest.cpp \
    src/params/SegmentationTest.cpp \
    src/params/ParamsTest.cpp \
    src/params/TransitionsTest.cpp \
    src/params/CDFTest.cpp \
    src/kernel/InputTest.cpp \
    src/kernel/XmlInputDataTest.cpp \
    src/kernel/InverseTest.cpp \
    \
    src/utils/Parser.cpp \
    src/utils/Logger.cpp \
    src/utils/Date.cpp \
    src/utils/Expr.cpp \
    src/utils/Exception.cpp \
    src/utils/ExpatParser.cpp \
    src/utils/ExpatUserData.cpp \
    src/utils/ExpatHandlers.cpp \
    src/utils/Utils.cpp \
    src/utils/Thread.cpp \
    src/utils/PowMatrix.cpp \
    src/portfolio/Obligor.cpp \
    src/portfolio/Asset.cpp \
    src/portfolio/DateValues.cpp \
    src/portfolio/LGD.cpp \
    src/portfolio/EAD.cpp \
    src/params/Factor.cpp \
    src/params/Rating.cpp \
    src/params/Params.cpp \
    src/params/Transitions.cpp \
    src/params/Interest.cpp \
    src/params/Segmentation.cpp \
    src/params/CDF.cpp \
    src/kernel/MonteCarlo.cpp \
    src/kernel/Input.cpp \
    src/kernel/InputData.cpp \
    src/kernel/XmlInputData.cpp \
    src/kernel/Aggregator.cpp \
    src/kernel/Inverse.cpp \
    src/kernel/SimulationThread.cpp \
    \
    src/utils/MiniCppUnit.hxx\
    src/utils/DateTest.hpp \
    src/utils/ExprTest.hpp \
    src/utils/ParserTest.hpp \
    src/utils/ExceptionTest.hpp \
    src/utils/UtilsTest.hpp \
    src/utils/ExpatUserDataTest.hpp \
    src/utils/PowMatrixTest.hpp \
    src/portfolio/AssetTest.hpp \
    src/portfolio/DateValuesTest.hpp \
    src/portfolio/LGDTest.hpp \
    src/portfolio/EADTest.hpp \
    src/portfolio/ObligorTest.hpp \
    src/params/InterestTest.hpp \
    src/params/SegmentationTest.hpp \
    src/params/ParamsTest.hpp \
    src/params/TransitionsTest.hpp \
    src/params/CDFTest.hpp \
    src/kernel/InputTest.hpp \
    src/kernel/XmlInputDataTest.hpp \
    src/kernel/InverseTest.hpp \
    \
    src/utils/Parser.hpp \
    src/utils/Logger.hpp \
    src/utils/Date.hpp \
    src/utils/Expr.hpp \
    src/utils/Exception.hpp \
    src/utils/ExpatParser.hpp \
    src/utils/ExpatUserData.hpp \
    src/utils/ExpatHandlers.hpp \
    src/utils/Utils.hpp \
    src/utils/Thread.hpp \
    src/utils/PowMatrix.hpp \
    src/portfolio/Asset.hpp \
    src/portfolio/DateValues.hpp \
    src/portfolio/LGD.hpp \
    src/portfolio/EAD.hpp \
    src/portfolio/Obligor.hpp \
    src/params/Factor.hpp \
    src/params/Rating.hpp \
    src/params/Params.hpp \
    src/params/Transitions.hpp \
    src/params/Interest.hpp \
    src/params/Segmentation.hpp \
    src/params/CDF.hpp \
    src/kernel/MonteCarlo.hpp \
    src/kernel/Input.hpp \
    src/kernel/InputData.hpp \
    src/kernel/XmlInputData.hpp \
    src/kernel/Aggregator.hpp \
    src/kernel/Inverse.hpp \
    src/kernel/SimulationThread.hpp \
    src/utils/config.h

#build_ccruncher_tests_CXXFLAGS =
#build_ccruncher_tests_LDADD =
#build_ccruncher_tests_LDFLAGS =

# -------------------------------------------------------------
# ccruncher-inf
# -------------------------------------------------------------
build_ccruncher_inf_SOURCES = \
    src/ccruncher-inf.cpp \
    src/utils/Exception.cpp \
    src/inference/Configuration.cpp \
    src/inference/Component.cpp \
    src/inference/Metropolis.cpp \
    \
    src/utils/Exception.hpp \
    src/inference/Configuration.hpp \
    src/inference/Component.hpp \
    src/inference/Metropolis.hpp \
    src/utils/config.h

#build_ccruncher_inf_CXXFLAGS = -fopenmp
#build_ccruncher_inf_LDADD =
#build_ccruncher_inf_LDFLAGS = -fopenmp
