
#***************************************************************************
#
# CreditCruncher - A portfolio credit risk valorator
# Copyright (C) 2004-2008 Gerard Torrent
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
# 2005/08/30 - Gerard Torrent [gerard@mail.generacio.com]
#   . initial release
#
# 2005/09/02 - Gerard Torrent [gerard@mail.generacio.com]
#   . added TCE support (Tail Conditional Expectation or Expected Shortfall)
#   . code refactored
#   . added usage example
#
# 2005/09/06 - Gerard Torrent [gerard@mail.generacio.com]
#   . removed portfolio value support
#
# 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
#   . added Rev svn:keyword
#
# 2007/07/27 - Gerard Torrent [gerard@mail.generacio.com]
#   . added breaks param in cmean(), cstddev() and plot()
#   . TCE renamed to ES
#
# 2008/11/26 - Gerard Torrent [gerard@mail.generacio.com]
#   . added rdigits argument in summary() function
#   . added function ccruncher.risk
#   . added function ccruncher.graphic
#   . added grid in graphics
#   . adapted to new output format (csv + multiple segments in the same file)
#   . renamed to ccreport.R
#
#***************************************************************************

#===========================================================================
# usage example
# --------------------------------------------------------------------------
# > source("bin/ccreport.R")                                 #load this R script
# > ccruncher.summary("data/portfolio.csv")                  #segmentation summary
# > ccruncher.graphic("data/portfolio.csv")                  #simple graphic
# > quit(save='no')                                          #quits R
#
# advanced functions
# --------------------------------------------------------------------------
# > source("bin/ccreport.R")                                 #load this R script
# > x <- ccruncher.read("sectors.csv")                       #load ccruncher data
# > names(x)                                                 #segment names
# > x[[1]]                                                   #list first column
# > ccruncher.risk(x[[1]])                                   #computes risk
# > ccruncher.plot(x[[1]], show="pdf")                       #plots a graphic
# > ccruncher.plot(x[[1]], show="cdf")                       #plots a graphic
# > ccruncher.plot(x[[1]], alpha=0.95, show="mean")          #plots a graphic
# > ccruncher.plot(x[[1]], alpha=0.95, show="stddev")        #plots a graphic
# > ccruncher.plot(x[[1]], alpha=0.95, var=0.99, show="VaR") #plots a graphic
# > ccruncher.plot(x[[1]], alpha=0.95, var=0.99, show="ES")  #plots a graphic
# > ccruncher.plot(x[[1]], alpha=0.95, var=0.99, show="all") #plots a graphic
#
#===========================================================================

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
#   Compute the requested ES (Expected Shortfall) and standar error for 
#   each Monte Carlo iteration
# arguments
#   x: vector with values
#   prob: numeric. quantile related to VaR. probability with value in [0,1]
#   breaks: numeric. number of evaluated values
# returns
#   matrix(1,): prob-es
#   matrix(2,): standard error of prob-es
# example
#   x <- rnorm(5000)
#   ccruncher.ces(x, 0.01)
# notes
#   when length(x) is large, this function is expensive
#   because involve calls to sort() function. breaks
#   argument allows you to reduce the number of computations
#===========================================================================
ccruncher.ces <- function(x, prob, breaks=250)
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
  plot(values[1,], type='l', ylim=yrange, panel.first = grid(),
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
#   writes a summary in xml format
# arguments
#   x: vector. simulated segment losses
#   percentiles: vector. VaR percentiles
# returns
#   list: risk statitstics
# example
#   df <- ccruncher.read("data/portfolio.csv")
#   risk <- ccruncher.summary(df[[1]])
#   risk
#   risk$n
#   risk$min
#   risk$max
#   risk$mean
#   risk$mean_stderr
#   risk$sd
#   risk$sd_stderr
#   risk$VAR
#   risk$ES
#===========================================================================
ccruncher.risk <- function(x, percentiles=c(0.90, 0.95, 0.975, 0.99, 0.9925, 0.995, 0.9975, 0.999, 0.9999))
{
  #VaR and ES tables (column1=percentile, column2=value, column3=stderr)
  table1 <- matrix(NaN, length(percentiles), 3);
  table2 <- matrix(NaN, length(percentiles), 3);

  #size, min, max
  n <- length(x);
  minx <- min(x);
  maxx <- max(x);

  #standar deviation and its standar error
  stddev <- sqrt(var(x));
  stderr2 <- stddev/sqrt(2*n);

  #mean and its standar error
  mu <- mean(x);
  stderr1 <- stddev/sqrt(n);

  #computing VaR (Value at Risk)
  for(i in 1:length(percentiles))
  {
    table1[i,1] <- percentiles[i];
    table1[i,2] <- quantile(x, percentiles[i], names=FALSE);
    table1[i,3] <- ccruncher.quantstderr(x, percentiles[i]);
  }

  #computing ES (Expected Shortfall)
  for(i in 1:length(percentiles))
  {
    #retrieving simulations with value great than VaR
    aux <- x[x >= table1[i,2]];
    #computing aux mean (=ES) and related stderr
    table2[i,1] <- percentiles[i];
    table2[i,2] <- mean(aux);
    table2[i,3] <- sqrt(var(aux))/sqrt(length(aux));
  }

  #exit function
  ret <- list(n=n, min=minx, max=maxx, 
              mean=mu, mean_stderr=stderr1, sd=stddev, sd_stderr=stderr2,
              VAR=table1, ES=table2);
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
#     - ES convergence
# arguments
#   x: vector with values
#   var: numeric. VaR level with value in [0,1]
#   alpha: numeric. confidence level with value in [0,1]
#   show: string. pdf|cdf|mean|stddev|VaR|ES|all
#   breaks: numeric. number of evaluated values in plots
# returns
#   the requested graphic
# example
#   x <- ccruncher.read("data/portfolio.csv")
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
    plot(aux, panel.first = grid(),
       main="Density Function",
       xlab="portfolio loss", ylab="probability");
  }
  if (show == "cdf" || show == "all") {
    plot.ecdf(x, verticals=TRUE, do.p=FALSE,
       panel.first = grid(), 
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
  if (show == "ES" || show == "all") {
    aux <- ccruncher.ces(x, prob=var, breaks=breaks);
    ccruncher.cplot(aux, alpha, "ES("%&%(var*100)%&%"%)");
  }
}

#===========================================================================
# description
#   reads a ccruncher csv output file
# arguments
#   filename: string. ccruncher output filename
# returns
#   a data frame with file content
# example
#   segments <- ccruncher.read("data/segments.csv")
#   names(segments)
#   segments[["S1"]]
#   segments[[1]]
#   ccruncher.summary(segments, "segments")
#===========================================================================
ccruncher.read <- function(filename)
{
  df <- read.csv(filename, header=TRUE, sep=",");
  return(df);
}

#===========================================================================
# description
#   given a csv ccruncher output file returns a risk summary
# arguments
#   filename: string. csv ccruncher output filename
#   format: string. allowed values: plain/xml
#   alpha: numeric. confidence level with value in (0,1)
#   rdigits: number of decimal digits showed in report
# returns
#   vector: each line is a text line of the summary
# example
#   lines <- ccruncher.summary("portfolio.csv", alpha=0.975)
#   write(lines, file="")
#===========================================================================
ccruncher.summary <- function(filename, format="plain", alpha=0.99, rdigits=2)
{
  #retrieves segmentation name from filename
  tokens <- strsplit(filename, "/");
  token <- tokens[[1]][length(tokens[[1]])];
  token <- strsplit(token, "\\.");
  segmentationname <- token[[1]][1];

  #read data file
  x <- ccruncher.read(filename);
  segmentnames <- names(x);

  #creating report object
  ret <- vector();

  for(i in 1:length(x))
  {
    #compute risk
    risk <- ccruncher.risk(x[[i]]);

    #prints risk
    if (format == "xml")
    {
      ret[length(ret)+1] <- "<segment name='" %&% segmentnames[i] %&% "'>";
      ret[length(ret)+1] <- "  <size value='" %&% risk$n %&% "' />";
      ret[length(ret)+1] <- "  <min value='" %&% risk$min %&% "' />";
      ret[length(ret)+1] <- "  <max value='" %&% risk$max %&% "' />";
      ret[length(ret)+1] <- "  <mean value='" %&% round(risk$mean,rdigits) %&% "' stderr='" %&% round(risk$mean_stderr,rdigits) %&% "' />";
      ret[length(ret)+1] <- "  <stddev value='" %&% round(risk$sd,rdigits) %&% "' stderr='" %&% round(risk$sd_stderr,rdigits) %&% "' />";
      for(i in 1:length(risk$VAR[,1]))
      {
        ret[length(ret)+1] <- "  <VaR prob='" %&% risk$VAR[i,1] %&%
                    "' value='" %&% round(risk$VAR[i,2],rdigits) %&% 
                    "' stderr='" %&% round(risk$VAR[i,3],rdigits) %&% "' />";
      }
      for(i in 1:length(risk$ES[,1]))
      {
        ret[length(ret)+1] <- "  <ES prob='" %&% risk$ES[i,1] %&%
                    "' value='" %&% round(risk$ES[i,2],rdigits) %&% 
                    "' stderr='" %&% round(risk$ES[i,3],rdigits) %&% "' />";
      }
      ret[length(ret)+1] <- "</segment>";
    }
    else
    {
      k <- qnorm((1-alpha)/2);
      ret[length(ret)+1] <- "";
      ret[length(ret)+1] <- "segment " %&% segmentnames[i] %&% ". summary at "%&%(alpha*100)%&%"% confidence level";
      ret[length(ret)+1] <- "---------------------------------------------------------------";
      ret[length(ret)+1] <- "n = " %&% risk$n;
      ret[length(ret)+1] <- "min = " %&% risk$min;
      ret[length(ret)+1] <- "max = " %&% risk$max;
      ret[length(ret)+1] <- "mean = " %&% round(risk$mean,rdigits) %&% " +/- " %&% round(abs(k*risk$mean_stderr),rdigits);
      ret[length(ret)+1] <- "stddev = " %&% round(risk$sd,rdigits) %&% " +/- " %&% round(abs(k*risk$sd_stderr),rdigits);
      for(i in 1:length(risk$VAR[,1]))
      {
        ret[length(ret)+1] <- "VAR(" %&% (risk$VAR[i,1]*100) %&% "%) = " %&% round(risk$VAR[i,2],rdigits) %&% " +/- " %&% round(abs(k*risk$VAR[i,3]),rdigits);
      }
      for(i in 1:length(risk$ES[,1]))
      {
        ret[length(ret)+1] <- "ES(" %&% (risk$ES[i,1]*100) %&% "%) = " %&% round(risk$ES[i,2],rdigits) %&% " +/- " %&% round(abs(k*risk$ES[i,3]),rdigits);
      }
    }

  }

  if (format=="xml")
  {
    ret <- c("<segmentation name='" %&% segmentationname %&% "'>", "  " %&% ret);
    ret <- c(ret, "</segmentation>");
    ret <- c("<ccruncher-report>", "  " %&% ret);
    ret <- c(ret, "</ccruncher-report>");
  }

  return(ret);
}

#===========================================================================
# description
#   creates a graphic for the given ccruncher output data
#   if only exists 1 segment -> plots density function
#   if more than 1 segment -> plots pie chart with expected loss per segment
# arguments
#   filename: string. csv ccruncher output filename
# returns
#   the requested graphic
# example
#   ccruncher.graphic("portfolio.csv")
#===========================================================================
ccruncher.graphic <- function(filename, rdigits=1)
{
  x <- ccruncher.read(filename);
  if (length(x) == 1)
  {
    aux <- density(x[[1]]);
    plot(aux, panel.first = grid(),
       main="Density Function",
       xlab="portfolio loss", ylab="probability");
  }
  else
  {
    txts <- names(x);
    vals <- vector(length=length(x));
    for(i in 1:length(x)) {
      vals[i] <- mean(x[[i]]);
    }
    for(i in 1:length(x)) {
      txts[i] <- txts[i] %&% " (" %&% round(100*vals[i]/sum(vals),rdigits) %&% "%)";
    }
    pie(vals, txts, main="Expected Loss by segment")
  }
}

