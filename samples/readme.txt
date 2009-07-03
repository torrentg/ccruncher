
CCRUNCHER TESTS
===============

This directory contains some xml input file samples for ccruncher.
You can run any file tipying next commands on a console:

  > cd $CCRUNCHER$
  > bin/ccruncher -vf --hash=100 --path=data samples/testXX.xml

The ouput will be written in files located at data directory

Currently, the checks are weak. Checks can be improved doing 
some tests (Chi-Square, Kolmogorov-Smirnof).

test04.xml
---------------------------------------------------------------------

Description:

  The same that test03, with these diferences:
    2 sectors
    correl(S1,S1) = 0.10
    correl(S1,S2) = 0.05
    correl(S2,S2) = 0.15
    Odds borrowers (50) belongs to S1 and even borrowers (50) belongs to S2

Expected Results:

  We don't know explicit solution for this problem (if you know, please
  report it).

   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
   0.00    6.00   10.00   10.78   14.00   49.00

How I can check results?

  > #loading script
  > source("bin/report.R")
  > #reading data
  > x <- ccruncher.read("data/portfolio.out")
  > #some results
  > summary(x)
  > #computed values
  > tabulate(x+1, 101)
  > #drawing histogram
  > hist(x,breaks=101)


test100.xml.gz
---------------------------------------------------------------------

Description:

  The same that test03, with these diferences:
    2 sectors
    correl(S1,S1) = 0.20
    correl(S1,S2) = 0.10
    correl(S2,S2) = 0.25
    100 borrowers
    each borrower with 3 assets
    24 time steps of 1 month length
    asset profile = 10 years bond. montly retribution. last payement = N(1000,x)
                    if borrower defaults, recovery=80% last payement.
    each borrower belongs to a random sector.
    interest spot curve defined

Expected Results:

  We don't know explicit solution for this problem (if you know, please
  report it).

   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
      0    1569    2970    3619    4936   34830

How I can check results?

  > #loading script
  > source("bin/report.R")
  > #reading data
  > x <- ccruncher.read("data/portfolio.out")
  > #some results
  > summary(x)
  > #computed values
  > tabulate(x+1, 101)
  > #drawing histogram
  > hist(x)


The CCruncher Team
http://www.generacio.com/ccruncher/
