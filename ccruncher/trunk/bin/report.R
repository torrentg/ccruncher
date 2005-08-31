
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
# report.R - report script for R (http://www.r-project.org)
# --------------------------------------------------------------------------
#
# 2005/08/30 - Gerard Torrent [gerard@fobos.generacio.com]
#   . initial release
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
#   Computes the standar error of a quantile quantile using
#   Maritz-Jarrett method
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
# returns
#   matrix(1,): mean
#   matrix(2,): standard error of mean
# example
#   x <- rnorm(5000)
#   ccruncher.cmean(x)
#===========================================================================
ccruncher.cmean <- function(x)
{
  #initializing values
  ret <- matrix(NaN, 2, length(x));
  ret[1,1] <- x[1];
  ret[2,1] <- 0;
  aux1 <- x[1];
  aux2 <- x[1]*x[1];

  #computing values
  for(i in 2:length(x))
  {
    aux1 <- aux1 + x[i];
    aux2 <- aux2 + x[i]*x[i];

    mu <- aux1/i;
    stddev <- sqrt((aux2-aux1*aux1/i)/(i-1));

    ret[1,i] <- mu;
    ret[2,i] <- stddev/sqrt(i);
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
# returns
#   matrix(1,): standard deviation
#   matrix(2,): standard error of standar deviation
# example
#   x <- rnorm(5000)
#   ccruncher.cstddev(x)
#===========================================================================
ccruncher.cstddev <- function(x)
{
  #initializing values
  ret <- matrix(NaN, 2, length(x));
  ret[1,1] <- 0;
  ret[2,1] <- 0;
  aux1 <- x[1];
  aux2 <- x[1]*x[1];

  #computing values
  for(i in 2:length(x))
  {
    aux1 <- aux1 + x[i];
    aux2 <- aux2 + x[i]*x[i];

    stddev <- sqrt((aux2-aux1*aux1/i)/(i-1));

    ret[1,i] <- stddev;
    ret[2,i] <- stddev/sqrt(2*i);
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
       ylab=name%&%" + "%&%(alpha*100)%&%"% confidence bound");
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
#   var: numeric. desired VaR level with value in [0,1]
#   alpha: numeric. confidence level with value in [0,1]
# returns
#   vector: each line is a text line of the summary
# example
#   x <- rnorm(5000)
#   lines <- ccruncher.summary(x, 0.99, 0.975)
#   write(lines, file="")
#===========================================================================
ccruncher.summary <- function(x, var, alpha)
{
  #computing summary values
  n <- length(x);
  mu <- mean(x);
  stddev <- sqrt(var(x));
  q <- quantile(x, 1-var, names=FALSE);
  qse <- ccruncher.quantstderr(x, 1-var);
  k <- qnorm((1-alpha)/2);

  #creating return object
  ret <- vector();

  #printing report
  ret[1] <- "";
  ret[2] <- "summary at "%&%(alpha*100)%&%"% confidence level";
  ret[3] <- "-------------------------------------------------------------------------";
  ret[4] <- "n = " %&% n;
  ret[5] <- "mean = " %&% mu %&% " +/- " %&% abs(k*stddev/sqrt(n));
  ret[6] <- "stddev = " %&% stddev %&% " +/- " %&% abs(k*stddev/sqrt(2*n));
  ret[7] <- "VAR(" %&% var %&% ") = " %&% q %&% " +/- " %&% abs(k*qse);
  ret[8] <- "";

  #exit function
  return(ret);
}

#===========================================================================
# description
#   computes the VaR (and stderr) for a selected group of VaR levels
# arguments
#   x: vector with values
# returns
#   vector(,1): VaR level with value in [0,1]
#   vector(,2): VaR value
#   vector(,3): VaR standard error
# example
#   x <- rnorm(5000)
#   vt <- ccruncher.vartable(x)
#   print(vt)
#===========================================================================
ccruncher.vartable <- function(x)
{
  #VaR values scaned
  xvar <- c(0.90, 0.95, 0.975, 0.99, 0.9925, 0.995, 0.9975, 0.999, 0.9999);

  #allocating return object (col1=VaR,col2=value,col3=stderr)
  ret <- matrix(NaN, length(xvar), 3)

  #computing values
  for(i in 1:length(xvar))
  {
    ret[i,1] <- xvar[i];
    ret[i,2] <- quantile(x, (1-xvar[i]));
    ret[i,3] <- ccruncher.quantstderr(x, (1-xvar[i]));
  }

  #return function
  return(ret)
}

#===========================================================================
# description
#   main function
# arguments
#   filename: string. ccruncher output filename
#   var: numeric. desired VaR level with value in [0,1]
#   alpha: numeric. confidence level with value in [0,1]
#   show: numeric. 1=full_graphics, 2=simple_graphics, 3=without graphics
# returns
#   some text and graphics in stdout
# example
#   source("bin/report.R")
#   ccruncher.main("data/portfolio-rest.out", var=0.99, alpha=0.99, show=3)
#   quit(save='no')
#===========================================================================
ccruncher.main <- function(filename, var=0.99, alpha=0.99, show=2)
{
  #checking arguments
  if (show < 1 | 3 < show)
  {
    return("invalid show value. choose one of 1/2/3");
  }

  #retrieving data from file
  df <- read.table(filename, col.names=c('index', 'value'));
  z <- as.vector(t(df["value"]));
  rm(df);

  if (show == 1)
  {
    #computing statistics evolution values
    ret1 <- ccruncher.cmean(z);
    ret2 <- ccruncher.cstddev(z);
    ret3 <- ccruncher.cquantile(z, prob=(1-var), breaks=250);

    #doing some graphics
    par(mfrow=c(2,2));
    plot(density(z));
    ccruncher.cplot(ret1, alpha, "Mean");
    ccruncher.cplot(ret2, alpha, "StdDev");
    ccruncher.cplot(ret3, alpha, "VaR("%&%(var*100)%&%"%)");
  }
  else if (show == 2)
  {
    #plotting density graph
    plot(density(z));
  }

  #computing values
  summary <- ccruncher.summary(z, var=var, alpha=alpha);
  vartable <- ccruncher.vartable(z);

  #printing results
  write("", file="");
  write(summary, file="");
  write("", file="");
  write("VaR table [col1=VaR-level, col2=VaR-value, col3=VaR-stderr]", file="");
  write("-------------------------------------------------------------------------", file="");
  write(format(t(vartable)), file="", ncolumns=3);
  write("", file="");
}
