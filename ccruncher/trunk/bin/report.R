
#***************************************************************************
#
# CreditCruncher - A portfolio credit risk valorator
# Copyright (C) 2004-2005 Gerard Torrent
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#
# report.R - report script for R (http://www.r-project.org) - $Rev$
# --------------------------------------------------------------------------
#
# 2005/08/30 - Gerard Torrent [gerard@fobos.generacio.com]
#   . initial release
#
# 2005/09/02 - Gerard Torrent [gerard@fobos.generacio.com]
#   . added TCE support (Tail Conditional Expectation or Expected Shortfall)
#   . code refactored
#   . added usage example
#
# 2005/09/06 - Gerard Torrent [gerard@fobos.generacio.com]
#   . removed portfolio value support
#
# 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
#   . added Rev svn:keyword
#
# 2007/07/17 - Gerard Torrent [gerard@mail.generacio.com]
#   . added breaks param in cmean(), cstddev() and plot()
#
#***************************************************************************

#===========================================================================
# description
#   concatenate 2 strings
# arguments
#   a: first string to concatenate
#   b: second string to concatenate
# returns
#   string: a concatenated with b
# example
#   print("i am a" %&% " sexy string")
#===========================================================================
"%&%" <- function(a, b) paste(a, b, sep="")

#===========================================================================
# description
#   Computes the standar error of a quantile using the Maritz-Jarrett method
# arguments
#   x: vector with values
#   prob: numeric. probability with value in [0,1]
#   sorted: boolean. TRUE={x is sorted}, FALSE={otherwise}
# returns
#   numeric: the standar error for the prob quantile
# example
#   x <- rnorm(5000)
#   ccruncher.quantstderr(x, 0.01)
#===========================================================================
ccruncher.quantstderr <- function(x, prob, sorted=FALSE)
{
  #sorting x
  if (sorted == FALSE) {
    y <- sort(x);
  }
  else {
    y <- x;
  }

  #initializing values
  N <- length(y);
  M <- trunc(N*prob+0.5);
  A <- M-1;
  B <- N-M;

  #checking feaseability
  if (A <= 0 || B <= 0) {
    return(NA);
  }

  #initializing coeficients
  W <- vector(length=N);
  W[] <- 0;

  #computing coeficients
  for(i in M:N)
  {
    W[i] <- pbeta((i+1)/N,A,B) - pbeta(i/N,A,B);
    if (W[i] < 1e-20) { break };
  }
  for(i in M:1)
  {
    W[i] <- pbeta((i+1)/N,A,B) - pbeta(i/N,A,B);
    if (W[i] < 1e-20) { break };
  }

  #computing C1 and C2
  C1 <- sum(W*y);
  C2 <- sum(W*y*y);

  #returning quantile standar error
  return(sqrt(C2-C1^2));
}

#===========================================================================
# description
#   Compute the mean and standar error for each
#   Monte Carlo iteration
# arguments
#   x: vector with values
#   breaks: numeric. number of evaluated values
# returns
#   matrix(1,): mean
#   matrix(2,): standard error of mean
# example
#   x <- rnorm(5000)
#   ccruncher.cmean(x)
#===========================================================================
ccruncher.cmean <- function(x, breaks=250)
{
  #initializing values
  ret <- matrix(NaN, 2, length(x));
  ret[1,1] <- x[1];
  ret[2,1] <- 0;
  aux1 <- x[1];
  aux2 <- x[1]*x[1];
  k <- max(1, as.integer(trunc(length(x)/breaks)));

  #computing values
  for(i in 2:length(x))
  {
    aux1 <- aux1 + x[i];
    aux2 <- aux2 + x[i]*x[i];

    if (i%%k == 0 | i >= length(x)-10)
    {
      mu <- aux1/i;
      stddev <- sqrt((aux2-aux1*aux1/i)/(i-1));
      ret[1,i] <- mu;
      ret[2,i] <- stddev/sqrt(i);
    }
    else
    {
      ret[1,i] <- ret[1,i-1];
      ret[2,i] <- ret[2,i-1];
    }
  }

  #returning values
  return(ret);
}

#===========================================================================
# description
#   Compute the standar deviation and standar error for each
#   Monte Carlo iteration
# arguments
#   x: vector with values
#   breaks: numeric. number of evaluated values
# returns
#   matrix(1,): standard deviation
#   matrix(2,): standard error of standar deviation
# example
#   x <- rnorm(5000)
#   ccruncher.cstddev(x)
#===========================================================================
ccruncher.cstddev <- function(x, breaks=250)
{
  #initializing values
  ret <- matrix(NaN, 2, length(x));
  ret[1,1] <- 0;
  ret[2,1] <- 0;
  aux1 <- x[1];
  aux2 <- x[1]*x[1];
  k <- max(1, as.integer(trunc(length(x)/breaks)));

  #computing values
  for(i in 2:length(x))
  {
    aux1 <- aux1 + x[i];
    aux2 <- aux2 + x[i]*x[i];

    if (i%%k == 0 | i >= length(x)-10)
    {
      stddev <- sqrt((aux2-aux1*aux1/i)/(i-1));
      ret[1,i] <- stddev;
      ret[2,i] <- stddev/sqrt(2*i);
    }
    else
    {
      ret[1,i] <- ret[1,i-1];
      ret[2,i] <- ret[2,i-1];
    }
  }

  #returning values
  return(ret);
}

#===========================================================================
# description
#   Compute the requested quantile and standar error for each
#   Monte Carlo iteration
# arguments
#   x: vector with values
#   prob: numeric. probability with value in [0,1]
#   breaks: numeric. number of evaluated values
# returns
#   matrix(1,): prob-quantile
#   matrix(2,): standard error of prob-quantile
# example
#   x <- rnorm(5000)
#   ccruncher.cquantile(x, 0.01)
# notes
#   when length(x) is large, this function is expensive
#   because involve calls to sort() function. breaks
#   argument allows you to reduce the number of computations
#===========================================================================
ccruncher.cquantile <- function(x, prob, breaks=250)
{
  #initializing values
  ret <- matrix(NA, 2, length(x));
  ret[1,1] <- 0;
  ret[2,1] <- 0;
  aux <- vector();
  aux[1] <- x[1];
  k <- max(1, as.integer(trunc(length(x)/breaks)));

  #computing values
  for(i in 2:length(x))
  {
    if (i%%k == 0 | i >= length(x)-10)
    {
      aux <- c(aux, x[(length(aux)+1):i]);
      aux <- sort(aux, method="sh");

      ret[1,i] <- quantile(aux[1:i], prob, names=FALSE);
      ret[2,i] <- ccruncher.quantstderr(aux[1:i], prob, sorted=TRUE);
    }
    else
    {
      ret[1,i] <- ret[1,i-1];
      ret[2,i] <- ret[2,i-1];
    }
  }

  #returning values
  return(ret);
}

#===========================================================================
# description
#   Compute the requested TCE (Tail Conditional Expectation or Expected 
#   Shortfall) and standar error for each Monte Carlo iteration
# arguments
#   x: vector with values
#   prob: numeric. quantile related to VaR. probability with value in [0,1]
#   breaks: numeric. number of evaluated values
# returns
#   matrix(1,): prob-tce
#   matrix(2,): standard error of prob-tce
# example
#   x <- rnorm(5000)
#   ccruncher.ctce(x, 0.01)
# notes
#   when length(x) is large, this function is expensive
#   because involve calls to sort() function. breaks
#   argument allows you to reduce the number of computations
#===========================================================================
ccruncher.ctce <- function(x, prob, breaks=250)
{
  #initializing values
  ret <- matrix(NA, 2, length(x));
  ret[1,1] <- 0;
  ret[2,1] <- 0;
  aux <- vector();
  aux[1] <- x[1];
  k <- max(1, as.integer(trunc(length(x)/breaks)));

  #computing values
  for(i in 2:length(x))
  {
    if (i%%k == 0 | i >= length(x)-10)
    {
      aux <- c(aux, x[(length(aux)+1):i]);
      aux <- sort(aux, method="sh");

      var <- quantile(aux[1:i], prob, names=FALSE);
      tmp <- aux[aux >= var];

      ret[1,i] <- mean(tmp);
      ret[2,i] <- sqrt(var(tmp))/sqrt(length(tmp));
    }
    else
    {
      ret[1,i] <- ret[1,i-1];
      ret[2,i] <- ret[2,i-1];
    }
  }

  #returning values
  return(ret);
}

#===========================================================================
# description
#   Plot the evolution (convergence) of a statistic
# arguments
#   values: matrix(2,n) where matrix(1,)=values and matrix(1,)=stderrs
#   alpha: numeric. confidence level with value in [0,1]
#   name: string. statistic name
# returns
#   a graphic
# example
#   x <- rnorm(5000)
#   m <- ccruncher.cmean(x)
#   ccruncher.cplot(m, 0.99, "mean")
#===========================================================================
ccruncher.cplot <- function(values, alpha, name="<name>")
{
  #retrieving length
  n <- length(values[1,]);

  #computing confidence level
  k <- qnorm((1-alpha)/2);

  #finding a pretty yrange
  yrange <- vector(length=2);
  yrange[1] <- values[1,n] - 7.5*abs(k*values[2,n]);
  yrange[2] <- values[1,n] + 7.5*abs(k*values[2,n]);

  #plotting statistic evolution
  plot(values[1,], type='l', ylim=yrange,
       main=name%&%" convergence",
       xlab="Monte Carlo iteration",
       ylab=name%&%" +/- "%&%(alpha*100)%&%"% confidence bound");
  par(new=TRUE);

  #plotting confidence levels bounds
  lines(values[1,]+k*values[2,], lty=3);
  lines(values[1,]-k*values[2,], lty=3);

  #horizontal line at last statistic value level
  abline(values[1,length(values[1,])],0);
}

#===========================================================================
# description
#   writes a summary
# arguments
#   x: vector with values
#   alpha: numeric. confidence level with value in [0,1]
#   format: string. plain={plain text}, xml={xml format}
# returns
#   vector: each line is a text line of the summary
# example
#   ccruncher.read("data/portfolio-rest.out")
#   lines <- ccruncher.summary(x, 0.99, format="xml")
#   write(lines, file="")
#===========================================================================
ccruncher.summary <- function(x, alpha=0.99, format="plain")
{
  #VaR values scaned (add your values here)
  xvar <- c(0.90, 0.95, 0.975, 0.99, 0.9925, 0.995, 0.9975, 0.999, 0.9999);
  table1 <- matrix(NaN, length(xvar), 3);
  table2 <- matrix(NaN, length(xvar), 3);

  #size, min, max, mean and stddev
  n <- length(x);
  minx <- min(x);
  maxx <- max(x);
  mu <- mean(x);
  stddev <- sqrt(var(x));

  #standar error for mean and stddev
  stderr1 <- stddev/sqrt(n);
  stderr2 <- stddev/sqrt(2*n);

  #computing VaR (Value at Risk)
  for(i in 1:length(xvar))
  {
    table1[i,1] <- xvar[i];
    table1[i,2] <- quantile(x, xvar[i], names=FALSE);
    table1[i,3] <- ccruncher.quantstderr(x, xvar[i]);
  }

  #computing TCE (Tail Conditional Expectation or Expected Shortfall)
  for(i in 1:length(xvar))
  {
    #retrieving simulations with value great than VaR
    aux <- x[x >= table1[i,2]];

    #coputing aux mean (=TCE) and related stderr
    table2[i,1] <- xvar[i];
    table2[i,2] <- mean(aux);
    table2[i,3] <- sqrt(var(aux))/sqrt(length(aux));
  }

  #factor used to compute confidence bounds
  k <- qnorm((1-alpha)/2);

  #creating return object
  ret <- vector();

  #printing summary
  if (format == "plain")
  {
    ret[length(ret)+1] <- "";
    ret[length(ret)+1] <- "portfolio loss summary at "%&%(alpha*100)%&%"% confidence level";
    ret[length(ret)+1] <- "---------------------------------------------------------------";
    ret[length(ret)+1] <- "n = " %&% n;
    ret[length(ret)+1] <- "min = " %&% minx;
    ret[length(ret)+1] <- "max = " %&% maxx;
    ret[length(ret)+1] <- "mean = " %&% mu %&% " +/- " %&% abs(k*stderr1);
    ret[length(ret)+1] <- "stddev = " %&% stddev %&% " +/- " %&% abs(k*stderr2);

    for(i in 1:length(table1[,1]))
    {
      ret[length(ret)+1] <- "VAR(" %&% (table1[i,1]*100) %&% "%) = " %&% table1[i,2] %&% " +/- " %&% abs(k*table1[i,3]);
    }

    for(i in 1:length(table2[,1]))
    {
      ret[length(ret)+1] <- "TCE(" %&% (table2[i,1]*100) %&% "%) = " %&% table2[i,2] %&% " +/- " %&% abs(k*table2[i,3]);
    }

    #adding indentation
    ret <- "  " %&% ret;
  }
  else if (format == "xml")
  {
    ret[length(ret)+1] <- "<ccruncher-report>";
    ret[length(ret)+1] <- "  <!-- ====================================================== -->";
    ret[length(ret)+1] <- "  <!-- confidence bounds can be computed as follows:          -->";
    ret[length(ret)+1] <- "  <!-- X = value +/- qnorm((1-alpha)/2) * stderr              -->";
    ret[length(ret)+1] <- "  <!-- where qnorm() is the inverse CDF for Normal(0,1) and   -->";
    ret[length(ret)+1] <- "  <!-- alpha is the desired confidence level (pe. alpha=0.99) -->";
    ret[length(ret)+1] <- "  <!-- ====================================================== -->";
    ret[length(ret)+1] <- "  <size value='" %&% n %&% "' />";
    ret[length(ret)+1] <- "  <min value='" %&% minx %&% "' />";
    ret[length(ret)+1] <- "  <max value='" %&% maxx %&% "' />";
    ret[length(ret)+1] <- "  <mean value='" %&% mu %&% "' stderr='" %&% stderr1 %&% "' />";
    ret[length(ret)+1] <- "  <stddev value='" %&% stddev %&% "' stderr='" %&% stderr2 %&% "' />";

    for(i in 1:length(table1[,1]))
    {
      ret[length(ret)+1] <- "  <VaR prob='" %&% table1[i,1] %&%
                  "' value='" %&% table1[i,2] %&% 
                  "' stderr='" %&% table1[i,3] %&% "' />";
    }

    for(i in 1:length(table2[,1]))
    {
      ret[length(ret)+1] <- "  <TCE prob='" %&% table2[i,1] %&%
                  "' value='" %&% table2[i,2] %&% 
                  "' stderr='" %&% table2[i,3] %&% "' />";
    }

    ret[length(ret)+1] <- "</ccruncher-report>";
  }
  else
  {
    return("error: incorrect format value. try plain or xml");
  }

  #exit function
  return(ret);
}

#===========================================================================
# description
#   plot some graphics related to ccruncher. These are:
#     - pdf plot (density)
#     - cdf plot (cumulative density)
#     - mean convergence
#     - stddev convergence
#     - VaR convergence
#     - TCE convergence
# arguments
#   x: vector with values
#   var: numeric. VaR level with value in [0,1]
#   alpha: numeric. confidence level with value in [0,1]
#   show: string. pdf|cdf|mean|stddev|VaR|TCE|all
#   breaks: numeric. number of evaluated values in plots
# returns
#   the requested graphic
# example
#   x <- ccruncher.read("data/portfolio-rest.out")
#   ccruncher.plot(x, alpha=0.95, var=0.99)
# notes
#   - confidence level is used to plot the confidence bounds on
#     convergence graphics.
#   - caution with convergence plots, can take some time to plot them
#===========================================================================
ccruncher.plot <- function(x, var=0.99, alpha=0.99, show="pdf", breaks=250)
{
  if (show == "all") { par(mfrow=c(3,2)); }

  if (show == "pdf" || show == "all") {
    aux <- density(x);
    plot(aux,
       main="Density Function",
       xlab="portfolio loss", ylab="probability");
  }
  if (show == "cdf" || show == "all") {
    plot.ecdf(x, verticals=TRUE, do.p=FALSE,
       main="Cumulative Distribution Function",
       xlab="portfolio loss", ylab="probability");
  }
  if (show == "mean" || show == "all") {
    aux <- ccruncher.cmean(x, breaks=breaks);
    ccruncher.cplot(aux, alpha, "Mean");
  }
  if (show == "stddev" || show == "all") {
    aux <- ccruncher.cstddev(x, breaks=breaks);
    ccruncher.cplot(aux, alpha, "StdDev");
  }
  if (show == "VaR" || show == "all") {
    aux <- ccruncher.cquantile(x, prob=var, breaks=breaks);
    ccruncher.cplot(aux, alpha, "VaR("%&%(var*100)%&%"%)");
  }
  if (show == "TCE" || show == "all") {
    aux <- ccruncher.ctce(x, prob=var, breaks=breaks);
    ccruncher.cplot(aux, alpha, "TCE("%&%(var*100)%&%"%)");
  }
}

#===========================================================================
# description
#   read a ccruncher output file and put values in a vector
# arguments
#   filename: string. ccruncher output filename
# returns
#   vector with values
# example
#   x <- ccruncher.read("data/portfolio-rest.out")
#   ccruncher.summary(x, alpha=0.95, format="plain")
#===========================================================================
ccruncher.read <- function(filename)
{
  #retrieving data from file
  df <- read.table(filename, col.names=c('index', 'value'));
  z <- as.vector(t(df["value"]));
  rm(df);

  #return function
  return(z);
}

#===========================================================================
# usage example
# --------------------------------------------------------------------------
# >
# > source("bin/report.R")                              #load R script
# > x <- ccruncher.load("file.out")                     #load data
# > x                                                   #list data
# > ccruncher.summary(x, alpha=0.95)                    #print summary
# > lines <- ccruncher.summary(x, format="xml")         #create xml summary
# > write(lines, file="")                               #print xml summary
# > ccruncher.plot(x, show="pdf")                       #plots a graphic
# > ccruncher.plot(x, show="cdf")                       #plots a graphic
# > ccruncher.plot(x, show="mean")                      #plots a graphic
# > ccruncher.plot(x, alpha=0.95, show="stddev")        #plots a graphic
# > ccruncher.plot(x, alpha=0.95, var=0.99, show="VaR") #plots a graphic
# > ccruncher.plot(x, alpha=0.95, var=0.99, show="TCE") #plots a graphic
# > ccruncher.plot(x, alpha=0.95, var=0.99, show="all") #plots a graphic
# > quit(save='no')                                     #quit R environement
#
#===========================================================================
