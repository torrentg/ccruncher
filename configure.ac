dnl *************************************************************
dnl
dnl Process this file with autoconf to produce a configure script
dnl 
dnl *************************************************************

dnl -------------------------------------------------------------
dnl initializing procedures
dnl -------------------------------------------------------------
AC_INIT(ccruncher, 2.5.0, gtorrent@ccruncher.net, ccruncher, http://www.ccruncher.net)
AC_CONFIG_SRCDIR([src/kernel/MonteCarlo.hpp])
AC_CONFIG_HEADER([src/utils/config.h],,[src/utils/config.h.in])
AM_INIT_AUTOMAKE([subdir-objects])

dnl -------------------------------------------------------------
dnl reset default CXXFLAGS value if not set by user
dnl -------------------------------------------------------------
: ${CXXFLAGS=""}

dnl -------------------------------------------------------------
dnl check for tools
dnl -------------------------------------------------------------
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_OPENMP

dnl -------------------------------------------------------------
dnl macros
dnl -------------------------------------------------------------
AC_LANG_PUSH([C++])
AX_CXX_COMPILE_STDCXX_14(noext,mandatory)

dnl -------------------------------------------------------------
dnl checks for libraries
dnl -------------------------------------------------------------
AC_CHECK_LIB([m], [cos])
AC_CHECK_HEADERS(expat.h,,
    [AC_MSG_ERROR([cannot find expat headers]) ])
AC_CHECK_LIB([expat], [XML_ParserCreate],,
    [AC_MSG_ERROR([cannot find the expat library]) ])
AC_CHECK_HEADERS(zlib.h,,
    [AC_MSG_ERROR([cannot find zlib headers]) ])
AC_CHECK_LIB([z], [zlibVersion],,
    [AC_MSG_ERROR([cannot find the zlib library]) ])
AC_CHECK_HEADERS(gsl/gsl_math.h,,
    [AC_MSG_ERROR([cannot find gsl headers]) ])
AC_CHECK_LIB([gslcblas],[cblas_dgemm],,
    [AC_MSG_ERROR([cannot find the gsl library]) ])
AC_CHECK_LIB([gsl], [gsl_rng_alloc],,
    [AC_MSG_ERROR([cannot find the gsl library]) ])
AC_CHECK_LIB([config++], [config_init],,
    [AC_MSG_ERROR([cannot find the libconfig library]) ])

dnl -------------------------------------------------------------
dnl checks for header
dnl -------------------------------------------------------------
AC_HEADER_STDBOOL
AC_CHECK_HEADERS([sys/time.h])

dnl -------------------------------------------------------------
dnl checks for functions
dnl -------------------------------------------------------------
AC_CHECK_FUNCS([getcwd])
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_FUNCS([mkdir])
AC_CHECK_FUNCS([pow])
AC_CHECK_FUNCS([realpath])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_FUNCS([strchr])
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([strtol])
AC_CHECK_FUNCS([strtoul])
AC_FUNC_STRTOD
AC_FUNC_MKTIME

dnl -------------------------------------------------------------
dnl other checks (consts, structs, types)
dnl -------------------------------------------------------------
AC_C_INLINE
AC_TYPE_SIZE_T

dnl -------------------------------------------------------------
dnl setting debug/optimization options
dnl -------------------------------------------------------------
AC_ARG_ENABLE(debug, 
  [  --enable-debug    Turn on debugging (default=no)],
  [case "${enableval}" in
    yes) debug=true ;;
    no)  debug=false ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
  esac],
  [debug=false])

if test x$debug = xtrue ; then
  AM_CXXFLAGS="-g"
else
  # note: consider -mtune=native -flto
  AM_CXXFLAGS="-O2 -DNDEBUG"
fi

dnl -------------------------------------------------------------
dnl setting profiler options
dnl   profiler mode => -pg
dnl -------------------------------------------------------------
AC_ARG_ENABLE(profiler,
  [  --enable-profiler    Turn on profiler (default=no)],
  [case "${enableval}" in
    yes) profiler=true ;;
    no)  profiler=false ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-profiler) ;;
  esac],
  [profiler=false])

if test x$profiler = xtrue ; then
  AM_CXXFLAGS="$AM_CXXFLAGS -pg"
  AC_DEFINE([PROFILER], 1, [Profiler instructions are added when this is defined.])
fi

dnl -------------------------------------------------------------
dnl setting compiler options
dnl -------------------------------------------------------------
AM_CXXFLAGS="$AM_CXXFLAGS -std=c++14 -pthread -fopenmp -Wall -Wextra -Wshadow -Wpedantic"
AC_SUBST(AM_CXXFLAGS)

dnl -------------------------------------------------------------
dnl setting preprocessor options
dnl -------------------------------------------------------------
AC_SUBST([AM_CPPFLAGS], ['-I$(top_srcdir)/src'])

dnl -------------------------------------------------------------
dnl defining some usefull variables in config.h
dnl -------------------------------------------------------------
AC_DEFINE_UNQUOTED([BUILD_USER], ["$(grep -P "^$(whoami):" /etc/passwd | cut -f5 -d:)"], [build user])
AC_DEFINE_UNQUOTED([BUILD_DATE], ["$(date --rfc-3339=seconds)"], [build date])
AC_DEFINE_UNQUOTED([BUILD_HOST], ["$(hostname | head -n 1)"], [build host name])

dnl -------------------------------------------------------------
dnl completing config.h.in
dnl -------------------------------------------------------------
AH_BOTTOM([

/* subversion identifier */
#define SVN_VERSION "R1367"

])

dnl -------------------------------------------------------------
dnl makefile's to be built
dnl -------------------------------------------------------------
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

