
This directory contains third part projects used by CreditCruncher.
Exist a subdirectory for each related project required at compilation time.
All related projects used by CreditCruncher are:

------------------------------------------------------------------------

project: minicppunit
version: 2.5
licence: GNU GPL
homeurl: http://minicppunit.sourceforge.net/ 
descrip: MiniCppUnit is a C++ unit testing framework. 
         In CreditCruncher is used to test all basic classes.
         All classes that match pattern *Test.?pp are unit test.
modific: Applied minor patch (reported to authors) related to encoding 
         (from ISO-8859-1 to UTF-8) and assertTrue(), assertTrueMissatge() methods.

------------------------------------------------------------------------

project: expat
version: 2.0.1
licence: MIT License
homeurl: http://expat.sourceforge.net/
descrip: Expat is an XML parser library written in C. It is a stream-oriented 
         parser in which an application registers handlers for things the 
         parser might find in the XML document (like start tags).
         In CreditCruncher is used to implements XML management.

------------------------------------------------------------------------

project: zlib
version: 1.2.3
licence: free license (see declaration at home site)
homeurl: http://www.zlib.net/
descrip: zlib is designed to be a free, general-purpose, legally unencumbered
         , that is, not covered by any patents -- lossless data-compression
         library for use on virtually any computer hardware and operating system.

------------------------------------------------------------------------

project: gzstream
version: 1.5
licence: GNU LGPL
homeurl: http://www.cs.unc.edu/Research/compgeom/gzstream/
descrip: Gzstream is a small C++ library, basically just a wrapper, that
         provides the functionality of the zlib C-library in a C++ iostream.

------------------------------------------------------------------------

project: jama/tnt
version: 1.2.5 (jama) + 1.2.6 (tnt)
licence: public domain (see declaration at home site)
homeurl: http://math.nist.gov/tnt/
descrip: The Template Numerical Toolkit (TNT) is a collection of interfaces 
         and reference implementations of numerical objects useful for 
         scientific computing in C++.
         In CreditCruncher is used to operate with matrix (pe. eigenvalues
         decomposition).

------------------------------------------------------------------------

project: MersenneTwister
version: 1.0
licence: BSD license
homeurl: http://www-personal.engin.umich.edu/~wagnerr/MersenneTwister.html
descrip: The Mersenne Twister is an algorithm for generating random numbers.
         In CreditCruncher is used to generate the random numbers used by
         MonteCarlo.

------------------------------------------------------------------------

project: valgrind
version: 3.3.0
licence: GNU GPL
homeurl: http://valgrind.kde.org
descrip: Valgrind is a GPL'd system for debugging and profiling x86-Linux 
         programs.
         In CreditCruncher is used to avoid memory leaks at development 
         time. No references into the code because valgrind don't need to
         be linked to code.

------------------------------------------------------------------------

project: mpi (parallel computing)
version: lam-mpi-7.1.2 
         open-mpi-1.1.8
licence: Lam-MPI: Indiana University Software License (see home site)
         Open-MPI: New BSD license
homeurl: http://www.lam-mpi.org/
         http://www.open-mpi.org/
descrip: LAM/MPI is a high-quality open-source implementation of the Message 
         Passing Interface specification, including all of MPI-1.2 and much 
         of MPI-2.
         Open MPI is an open source, freely available implementation of both 
         the MPI-1 and MPI-2 documents. The Open MPI software achieves high 
         performance; the Open MPI project is quite receptive to community 
         input.
         In CreditCruncher is used to implements parallel execution.

------------------------------------------------------------------------

project: R
version: 2.7.0
licence: GNU GPL
homeurl: http://www.r-project.org/
descrip: R is a system for statistical computation and graphics. It consists
         of a language plus a run-time environment with graphics, a debugger,
         access to certain system functions, and the ability to run programs
         stored in script files.

------------------------------------------------------------------------
