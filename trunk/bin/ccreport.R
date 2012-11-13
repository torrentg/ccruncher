
#***************************************************************************
#
# CreditCruncher - A portfolio credit risk valorator
# Copyright (C) 2004-2012 Gerard Torrent
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
# > x[,1]                                                    #list first column
# > ccruncher.risk(x[,1])                                    #computes risk
# > ccruncher.plot(x[,1], show="pdf")                        #plots a graphic
# > ccruncher.plot(x[,1], show="cdf")                        #plots a graphic
# > ccruncher.plot(x[,1], alpha=0.95, show="mean")           #plots a graphic
# > ccruncher.plot(x[,1], alpha=0.95, show="stddev")         #plots a graphic
# > ccruncher.plot(x[,1], alpha=0.95, var=0.99, show="VaR")  #plots a graphic
# > ccruncher.plot(x[,1], alpha=0.95, var=0.99, show="ES")   #plots a graphic
# > ccruncher.plot(x[,1], alpha=0.95, var=0.99, show="all")  #plots a graphic
#
#===========================================================================

#===========================================================================
# description
#   Internal function
#   Concatenate 2 strings
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
#   Internal function
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
    if (W[i] < 1e-10) { break };
  }
  for(i in (M-1):1)
  {
    W[i] <- pbeta((i+1)/N,A,B) - pbeta(i/N,A,B);
    if (W[i] < 1e-10) { break };
  }

  #computing C1 and C2
  C1 <- sum(W*y);
  C2 <- sum(W*y*y);

  #returning quantile standar error
  return(sqrt(C2-C1^2));
}

#===========================================================================
# description
#   Internal function
#   Compute the mean and standar error for each
#   Monte Carlo iteration
# arguments
#   x: vector with values
#   breaks: vector with the indexes where to compute the mean
# returns
#   matrix(1,): mean
#   matrix(2,): standard error of mean
# example
#   x <- rnorm(5000)
#   breaks <- seq(0,5000,250)
#   breaks[1] <- 2
#   ccruncher.cmean(x, breaks)
#===========================================================================
ccruncher.cmean <- function(x, breaks)
{
  #checking parameters
  if (!is.vector(x) | length(x)<2) {
    stop("not valid x argument");
  }
  if (!is.vector(breaks) | length(breaks)<1) {
    stop("not valid breaks argument");
  }
  if (length(breaks[breaks<2]) > 0) {
    stop("breaks with a value less than 2");
  }
  if (length(breaks[breaks>length(x)]) > 0) {
    stop("breaks with a value bigger than x length");
  }
  aux <- tabulate(breaks);
  if (length(aux[aux>1])>0) {
    stop("breaks with repeated values");
  }
  sort(breaks);

  #initializing values
  ret <- matrix(NaN, 2, length(breaks));
  sum1 <- 0;
  sum2 <- 0;

  #computing values
  for(i in 1:length(breaks))
  {
    n1 <- ifelse(i==1,1,breaks[i-1]+1);
    n2 <- breaks[i];
    sum1 <- sum1 + sum(x[n1:n2]);
    sum2 <- sum2 + sum(x[n1:n2]^2);
    mu <- sum1/n2;
    stddev <- sqrt((sum2-sum1*sum1/n2)/(n2-1));
    ret[1,i] <- mu;
    ret[2,i] <- stddev/sqrt(n2);
  }

  #returning values
  return(ret);
}

#===========================================================================
# description
#   Internal function
#   Compute the standar deviation and standar error for each
#   Monte Carlo iteration
# arguments
#   x: vector with values
#   breaks: vector with the indexes where to compute the mean
# returns
#   matrix(1,): standard deviation
#   matrix(2,): standard error of standar deviation
# example
#   x <- rnorm(5000)
#   breaks <- seq(0,5000,250)
#   breaks[1] <- 2
#   ccruncher.cstddev(x, breaks)
#===========================================================================
ccruncher.cstddev <- function(x, breaks)
{
  #checking parameters
  if (!is.vector(x) | length(x)<2) {
    stop("not valid x argument");
  }
  if (!is.vector(breaks) | length(breaks)<1) {
    stop("not valid breaks argument");
  }
  if (length(breaks[breaks<2]) > 0) {
    stop("breaks with a value less than 2");
  }
  if (length(breaks[breaks>length(x)]) > 0) {
    stop("breaks with a value bigger than x length");
  }
  aux <- tabulate(breaks);
  if (length(aux[aux>1])>0) {
    stop("breaks with repeated values");
  }
  sort(breaks);

  #initializing values
  ret <- matrix(NaN, 2, length(breaks));
  sum1 <- 0;
  sum2 <- 0;

  #computing values
  for(i in 1:length(breaks))
  {
    n1 <- ifelse(i==1,1,breaks[i-1]+1);
    n2 <- breaks[i];
    sum1 <- sum1 + sum(x[n1:n2]);
    sum2 <- sum2 + sum(x[n1:n2]^2);
    mu <- sum1/n2;
    stddev <- sqrt((sum2-sum1*sum1/n2)/(n2-1));
    ret[1,i] <- stddev;
    ret[2,i] <- stddev/sqrt(2*n2);
  }

  #returning values
  return(ret);
}

#===========================================================================
# description
#   Internal function
#   Compute the requested quantile and standar error for each
#   Monte Carlo iteration
# arguments
#   x: vector with values
#   prob: numeric. probability with value in [0,1]
#   breaks: vector with the indexes where to compute the mean
# returns
#   matrix(1,): prob-quantile
#   matrix(2,): standard error of prob-quantile
# example
#   x <- rnorm(5000)
#   breaks <- seq(0,5000,250)
#   breaks[1] <- 2
#   ccruncher.cquantile(x, breaks)
# notes
#   when length(x) is large, this function is expensive
#   because involve calls to sort() function
#===========================================================================
ccruncher.cquantile <- function(x, prob, breaks)
{
  #checking parameters
  if (!is.vector(x) | length(x)<2) {
    stop("not valid x argument");
  }
  if (!is.vector(breaks) | length(breaks)<1) {
    stop("not valid breaks argument");
  }
  if (length(breaks[breaks<2]) > 0) {
    stop("breaks with a value less than 2");
  }
  if (length(breaks[breaks>length(x)]) > 0) {
    stop("breaks with a value bigger than x length");
  }
  aux <- tabulate(breaks);
  if (length(aux[aux>1])>0) {
    stop("breaks with repeated values");
  }
  sort(breaks);

  #initializing values
  ret <- matrix(NaN, 2, length(breaks));
  sum1 <- 0;
  sum2 <- 0;

  #computing values
  for(i in 1:length(breaks))
  {
    n1 <- ifelse(i==1,1,breaks[i-1]+1);
    n2 <- breaks[i];
    x[1:n2] <- sort(x[1:n2]);
    ret[1,i] <- quantile(x[1:n2], prob, names=FALSE);
    ret[2,i] <- ccruncher.quantstderr(x[1:n2], prob, sorted=TRUE);
  }

  #returning values
  return(ret);
}

#===========================================================================
# description
#   Internal function
#   Compute the requested ES (Expected Shortfall) and standar error for 
#   each Monte Carlo iteration
# arguments
#   x: vector with values
#   prob: numeric. quantile related to VaR. probability with value in [0,1]
#   breaks: vector with the indexes where to compute the mean
# returns
#   matrix(1,): prob-es
#   matrix(2,): standard error of prob-es
# example
#   x <- rnorm(5000)
#   breaks <- seq(0,5000,250)
#   breaks[1] <- 2
#   ccruncher.ces(x, breaks)
# notes
#   when length(x) is large, this function is expensive
#   because involve calls to sort() function
#===========================================================================
ccruncher.ces <- function(x, prob, breaks)
{
  #checking parameters
  if (!is.vector(x) | length(x)<2) {
    stop("not valid x argument");
  }
  if (!is.vector(breaks) | length(breaks)<1) {
    stop("not valid breaks argument");
  }
  if (length(breaks[breaks<2]) > 0) {
    stop("breaks with a value less than 2");
  }
  if (length(breaks[breaks>length(x)]) > 0) {
    stop("breaks with a value bigger than x length");
  }
  aux <- tabulate(breaks);
  if (length(aux[aux>1])>0) {
    stop("breaks with repeated values");
  }
  sort(breaks);

  #initializing values
  ret <- matrix(NaN, 2, length(breaks));

  #computing values
  for(i in 1:length(breaks))
  {
    n1 <- ifelse(i==1,1,breaks[i-1]+1);
    n2 <- breaks[i];
    x[1:n2] <- sort(x[1:n2]);
    q <- quantile(x[1:n2], prob, names=FALSE);
    tmp <- x[1:n2][x[1:n2]>=q];
    ret[1,i] <- mean(tmp);
    ret[2,i] <- sqrt(var(tmp))/sqrt(length(tmp));
  }

  #returning values
  return(ret);
}

#===========================================================================
# description
#   Internal function
#   Plot the evolution (convergence) of a statistic
# arguments
#   values: matrix(2,n) where matrix(1,)=values and matrix(1,)=stderrs
#   alpha: numeric. confidence level with value in [0,1]
#   name: string. statistic name
# returns
#   a graphic
# example
#   x <- seq(0,5000,250)
#   x[1] <- 2
#   data <- rnorm(5000)
#   y <- ccruncher.cmean(data, x)
#   ccruncher.cplot(x, y, 0.99, "mean")
#===========================================================================
ccruncher.cplot <- function(x, y, alpha, name="<name>")
{
  #retrieving length
  n <- length(y[1,]);

  #computing confidence level
  k <- qnorm((1-alpha)/2);

  #finding a pretty yrange
  yrange <- vector(length=2);
  yrange[1] <- y[1,n] - 7.5*abs(k*y[2,n]);
  yrange[2] <- y[1,n] + 7.5*abs(k*y[2,n]);

  #plotting statistic evolution
  plot(x, y[1,], type='l', ylim=yrange, panel.first = grid(),
       main=name%&%" convergence",
       xlab="Monte Carlo iteration",
       ylab=name%&%" +/- "%&%(alpha*100)%&%"% confidence bound");
  par(new=TRUE);

  #plotting confidence levels bounds
  lines(x, y[1,]+k*y[2,], lty=3);
  lines(x, y[1,]-k*y[2,], lty=3);

  #horizontal line at last statistic value level
  abline(y[1,length(y[1,])],0);
}

#===========================================================================
# description
#   Computes risk indicators
# arguments
#   x: vector. simulated segment losses
#   percentiles: vector. VaR percentiles
# returns
#   list: risk statistics
# example
#   df <- ccruncher.read("data/portfolio.csv")
#   risk <- ccruncher.summary(df[,1])
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
#   plot some graphics related to ccruncher:
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
#   xres: numeric. number of evaluated values in plots
# returns
#   the requested graphic
# example
#   x <- ccruncher.read("data/portfolio.csv")
#   ccruncher.plot(x[,1], alpha=0.95, var=0.99)
# notes
#   - confidence level is used to plot the confidence bounds on
#     convergence graphics.
#   - caution with convergence plots, can take some time to plot them
#===========================================================================
ccruncher.plot <- function(x, var=0.99, alpha=0.99, show="pdf", xres=250)
{
  #variables definition
  plotoptions <- c("all", "pdf", "cdf", "mean", "stddev", "VaR", "ES");

  #checking parameters
  if (!is.vector(x) | length(x)<2) {
    stop("not valid x argument");
  }
  if (var <= 0 | var >= 1) {
    stop("var value out of range (0,1)");
  }
  if (alpha <= 0 | alpha >= 1) {
    stop("alpha value out of range (0,1)");
  }
  if (length(plotoptions[plotoptions==show]) == 0) {
    stop("unrecognized show option");
  }
  if (xres < 2) {
    stop("xres out of range");
  }

  #fixing evaluation points
  breaks <- seq(0,length(x),trunc(length(x)/xres));
  breaks[1] <- 2;
  if (breaks[length(breaks)] != length(x)) breaks <- c(breaks,length(x));

  #multiple plots in the graphics
  if (show == "all") { par(mfrow=c(3,2)); }

  #create the graphic
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
    y <- ccruncher.cmean(x, breaks);
    ccruncher.cplot(breaks, y, alpha, "Mean");
  }
  if (show == "stddev" || show == "all") {
    y <- ccruncher.cstddev(x, breaks);
    ccruncher.cplot(breaks, y, alpha, "StdDev");
  }
  if (show == "VaR" || show == "all") {
    y <- ccruncher.cquantile(x, prob=var, breaks);
    ccruncher.cplot(breaks, y, alpha, "VaR("%&%(var*100)%&%"%)");
  }
  if (show == "ES" || show == "all") {
    y <- ccruncher.ces(x, prob=var, breaks);
    ccruncher.cplot(breaks, y, alpha, "ES("%&%(var*100)%&%"%)");
  }
}

#===========================================================================
# description
#   reads a ccruncher csv output file
# arguments
#   filename: string. csv ccruncher output filename
# returns
#   a data frame with file content
# example
#   segments <- ccruncher.read("data/segments.csv")
#   names(segments)
#   segments[,"S1"]
#   segments[,1]
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
    risk <- ccruncher.risk(x[,i]);

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
#   rdigits: number of decimal digits
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
    aux <- density(x[,1]);
    plot(aux, panel.first = grid(),
       main="Density Function",
       xlab="portfolio loss", ylab="probability");
  }
  else
  {
    txts <- names(x);
    vals <- vector(length=length(x));
    for(i in 1:length(x)) {
      vals[i] <- mean(x[,i]);
    }
    for(i in 1:length(x)) {
      txts[i] <- txts[i] %&% " (" %&% round(100*vals[i]/sum(vals),rdigits) %&% "%)";
    }
    pie(vals, txts, main="Expected Loss by segment")
  }
}

#===========================================================================
# description
#   plots a density chart
# arguments
#   x: vector with simulated values
# returns
#   the requested density chart
# example
#   x <- ccruncher.read("file.csv");
#   ccruncher.density(x[,1])
#===========================================================================
ccruncher.density <- function(x)
{
  aux <- density(x);
  plot(aux, panel.first = grid(),
     main="Density Function",
     xlab="default loss", ylab="probability");
}

#===========================================================================
# description
#   creates a pie chart with expected loss per segment
# arguments
#   filename: string. csv ccruncher output filename
#   rdigits: number of decimal digits
# returns
#   the requested graphic
# example
#   ccruncher.piechart("file.csv")
#===========================================================================
ccruncher.piechart <- function(filename, rdigits=1)
{
  x <- ccruncher.read(filename);
  txts <- names(x);
  vals <- vector(length=length(x));
  for(i in 1:length(x)) {
    vals[i] <- mean(x[,i]);
  }
  for(i in 1:length(x)) {
    txts[i] <- txts[i] %&% " (" %&% round(100*vals[i]/sum(vals),rdigits) %&% "%)";
  }
  pie(vals, txts, main="Expected Loss by segment")
}

