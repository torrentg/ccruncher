
CREDITCRUNCHER SAMPLES
======================

This directory contains some xml input file samples
for ccruncher solver. You can run any sample file
tipying next commands on a console:

  > cd $CCRUNCHER$
  > bin/ccruncher -vf --hash=100 --path=data samples/sampleXX.xml

The ouput will be written in files located at data directory.


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
  1 time step of 12 months length
  The client has an unique asset with this profile:
    asset value at t=12 is 1 if client is alive and
    asset value at t=12 is 0 if client dead (default)
  time-to-default resolution method

Expected Results:

  This problem has an explicit solution. The asset value
  at t=12 can be modeled as a Bernouilli(0.9). Then, the
  portfolio value at t=12 can be modeled as Z~Bernouilli(0.9)

     P(Z=0) = 0.1
     P(Z=1) = 0.9


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
  1 time step of 12 months length
  Each client has an unique asset with this profile:
    asset value at t=12 is 1 if client is alive and
    asset value at t=12 is 0 if client dead (default)
  time-to-default resolution method

Expected Results:

  This problem has an explicit solution. Each asset value
  at t=12 can be modeled as a Bernouilli(0.9). Then, the
  portfolio value at t=12 can be modeled as:

     P(Z=0) = P(X1=0)·P(X2=0) = 0.1 · 0.1 = 0.01
     P(Z=1) = P(X1=1)·P(X2=0) + P(X1=0)·P(X2=1) = 0.9 · 0.1 + 0.1 · 0.9 = 0.18
     P(Z=2) = P(X1=1)·P(X2=1) = 0.9 · 0.9 = 0.81


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
  1 time step of 12 months length
  Each client has an asset with this profile:
    asset value at t=12 is 1 if client is alive and
    asset value at t=12 is 0 if client dead (default)
  time-to-default resolution method

Expected Results:

  This problem has an explicit solution. The asset value
  at t=12 for each clieant can be modeled as a Bernouilli(0.9).
  The sum of 100 uncorrelated Bernouillis is a Binomial(100,0.9).
  Then, the portfolio value at t=12 is a Z~Binomial(100,0.9).

  We can compute exact results using octave:
    > x=[75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,95,96,97,98,99,100]'
    > binomial_pdf(x,100,0.9)
    > format none
    > [x,binomial_pdf(x,100,0.9)]

         75  8.9729337195651e-06
         76  2.6564606406607e-05
         77  7.4518895893858e-05
         78  0.00019776168525679
         79  0.00049565586937776
         80  0.0011709869914049
         81  0.0026021933142331
         82  0.0054265250821204
         83  0.010591530883175
         84  0.019291716965783
         85  0.032682438153798
         86  0.05130382733445
         87  0.074302094760235
         88  0.098788012351673
         89  0.11987758802226
         90  0.13186534682449
         91  0.13041627707916
         92  0.11482302655882
         93  0.088895246368124
         95  0.033865803823556
         96  0.015874595542292
         97  0.0058916024693042
         98  0.0016231965986858
         99  0.00029512665430652
        100  2.6561398887588e-05

  Take simulated portfolio values output file (portfolio-rest.out),
  compute pdf (probability density function) at 75,...,100 an compare
  with binomial values.


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
  report it). We do an artistic description of pdf compared with sample03:

                               sample03  sample04
    simetric                     very      none
    maximum probability           93        94
    minimum                       80        60
    P(Z=100)                      0.0       0.1




The CreditCruncher Team
http://www.generacio.com/ccruncher/
