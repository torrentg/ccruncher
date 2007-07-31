
CREDITCRUNCHER SAMPLES
======================

This directory contains some xml input file samples for ccruncher.
You can run any sample file tipying next commands on a console:

  > cd $CCRUNCHER$
  > bin/ccruncher -vf --hash=100 --path=data samples/sampleXX.xml

The ouput will be written in files located at data directory

Currently, the checks are weak. Checks can be improved doing 
some tests (Chi-Square, Kolmogorov-Smirnof).


sample01.xml
---------------------------------------------------------------------

Description:

  1 client
  1 unique sector
  uncorrelated defaults (due to only exist 1 client)
  2 ratings (dead and alive)
  transition values:
    P(alive->alive) = 0.9
    P(alive->dead)  = 0.1
    P(dead->alive)  = 0.0
    P(dead->dead)   = 1.0
  12 time steps of 1 month length
  The client has an unique asset with this profile:
    asset value at t=12 is 1 if client is alive and
    asset value at t=12 is 0 if client dead (default)
  time-to-default resolution method

Expected Results:

  This problem has an explicit solution. The asset loss
  at t=12 can be modeled as a Bernouilli(0.1). Then, the
  portfolio loss at t=12 can be modeled as Z~Bernouilli(0.1)

     P(Z=0) = 0.9
     P(Z=1) = 0.1

How I can check results?

  > #loading script
  > source("bin/report.R")
  > #reading data
  > x <- ccruncher.read("data/portfolio-rest.out")
  > #exact values
  > c(0.9, 0.1)*length(x)
  > #computed values
  > tabulate(x+1, 2)


sample02.xml
---------------------------------------------------------------------

Description:

  2 clients
  1 unique sector
  uncorrelated defaults
  2 ratings (dead and alive)
  transition values:
    P(alive->alive) = 0.9
    P(alive->dead)  = 0.1
    P(dead->alive)  = 0.0
    P(dead->dead)   = 1.0
  12 time steps of 1 month length
  Each client has an unique asset with this profile:
    asset value at t=12 is 1 if client is alive and
    asset value at t=12 is 0 if client dead (default)
  time-to-default resolution method

Expected Results:

  This problem has an explicit solution. Each asset loss
  at t=12 can be modeled as a Bernouilli(0.1). Then, the
  portfolio loss at t=12 can be modeled as:

     P(Z=0) = P(X1=0)·P(X2=0) = 0.9 · 0.9 = 0.81
     P(Z=1) = P(X1=0)·P(X2=1) + P(X1=1)·P(X2=0) = 0.9 · 0.1 + 0.1 · 0.9 = 0.18
     P(Z=2) = P(X1=1)·P(X2=1) = 0.1 · 0.1 = 0.01

How I can check results?

  > #loading script
  > source("bin/report.R")
  > #reading data
  > x <- ccruncher.read("data/portfolio-rest.out")
  > #exact values
  > c(0.9*0.9, 0.9*0.1+0.1*0.9, 0.1*0.1)*length(x)
  > #computed values
  > tabulate(x+1, 3)


sample03.xml
---------------------------------------------------------------------

Description:

  100 clients
  1 unique sector
  uncorrelated defaults
  2 ratings (dead and alive)
  transition values:
    P(alive->alive) = 0.9
    P(alive->dead)  = 0.1
    P(dead->alive)  = 0.0
    P(dead->dead)   = 1.0
  12 time steps of 1 month length
  Each client has an asset with this profile:
    asset value at t=12 is 1 if client is alive and
    asset value at t=12 is 0 if client dead (default)
  time-to-default resolution method

Expected Results:

  This problem has an explicit solution. The asset loss
  at t=12 for each clieant can be modeled as a Bernouilli(0.1).
  The sum of 100 uncorrelated Bernouillis is a Binomial(100,0.1).
  Then, the portfolio loss at t=12 is a Z~Binomial(100,0.1).

How I can check results?

  > #loading script
  > source("bin/report.R")
  > #reading data
  > x <- ccruncher.read("data/portfolio-rest.out")
  > #exact values
  > round(dbinom(0:100, 100, 0.1)*length(x))
  > #computed values
  > tabulate(x+1, 101)


sample04.xml
---------------------------------------------------------------------

Description:

  The same that sample03, with these diferences:
    2 sectors
    correl(S1,S1) = 0.10
    correl(S1,S2) = 0.05
    correl(S2,S2) = 0.15
    Odds clients (50) belongs to S1 and even clients (50) belongs to S2

Expected Results:

  We don't know explicit solution for this problem (if you know, please
  report it).

   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
   0.00    6.00   10.00   10.78   14.00   49.00

How I can check results?

  > #loading script
  > source("bin/report.R")
  > #reading data
  > x <- ccruncher.read("data/portfolio-rest.out")
  > #some results
  > summary(x)
  > #computed values
  > tabulate(x+1, 101)
  > #drawing histogram
  > hist(x,breaks=101)


sample100.xml.gz
---------------------------------------------------------------------

Description:

  The same that sample03, with these diferences:
    2 sectors
    correl(S1,S1) = 0.20
    correl(S1,S2) = 0.10
    correl(S2,S2) = 0.25
    100 clients
    each client with 3 assets
    24 time steps of 1 month length
    asset profile = 10 years bond. montly retribution. last payement = N(1000,x)
                    if client defaults, recovery=80% last payement.
    each client belongs to a random sector.
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
  > x <- ccruncher.read("data/portfolio-rest.out")
  > #some results
  > summary(x)
  > #computed values
  > tabulate(x+1, 101)
  > #drawing histogram
  > hist(x)


The CreditCruncher Team
http://www.generacio.com/ccruncher/
