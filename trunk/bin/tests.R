
#***************************************************************************
#
# CreditCruncher - A portfolio credit risk valorator
# Copyright (C) 2004-2014 Gerard Torrent
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
#   checks that two segmentations sums the same
# arguments
#   segmentation1: segmentation1 data frame
#   segmentation2: segmentation2 data frame
# returns
#   TRUE=ok, FALSE=otherwise
# example
#   isCompletete(portfolio, sectors)
#===========================================================================
isComplete <- function(segmentation1, segmentation2)
{
  if (length(segmentation1) <= 0 || 
      length(segmentation2) <= 0 || 
      length(segmentation1[[1]]) != length(segmentation2[[1]]) )
  {
    return(FALSE);
  }
  
  # maximum error allowed due to rounding
  epsilon <- 0.01*length(segmentation1) + 0.00001
  
  aux <- apply(segmentation1,1,sum) - apply(segmentation2,1,sum)
  if (length(aux[aux>epsilon])) { 
    return(FALSE);
  }

  #returning values
  return(TRUE);
}

#===========================================================================
# test01
#===========================================================================
test01 <- function()
{
  cat("test01\n");
  
  #reading data
  portfolio <- ccruncher.read("data/test01/portfolio.csv");
  sectors <- ccruncher.read("data/test01/sectors.csv");

  #checking segmention completeness
  cat("  segmentations consistency: ")
  if (isComplete(portfolio, sectors)) {
    cat("OK\n");
  } else {
    cat("FAILED\n"); 
  }

  #checking loss distribution
  cat("  loss distribution: ")
  obs <- tabulate(portfolio[,1]+1, 2);
  prob <- c(0.9, 0.1);
  cst <- chisq.test(obs, p=prob);
  if (cst$p.value > 0.1) { 
    cat("OK\n");
  } else { 
    cat("FAILED\n"); 
  }
  
}

#===========================================================================
# test02
#===========================================================================
test02 <- function()
{
  cat("test02\n");
  
  #reading data
  portfolio <- ccruncher.read("data/test02/portfolio.csv");

  #checking loss distribution
  cat("  loss distribution: ")
  obs <- tabulate(portfolio[,1]+1, 3);
  prob <- c(0.9*0.9, 0.9*0.1+0.1*0.9, 0.1*0.1);
  cst <- chisq.test(obs, p=prob)
  if (cst$p.value > 0.1) { 
    cat("OK\n");
  } else { 
    cat("FAILED\n"); 
  }

}

#===========================================================================
# test03
#===========================================================================
test03 <- function()
{
  cat("test03\n");
  
  #reading data
  portfolio <- ccruncher.read("data/test03/portfolio.csv");

  #checking loss distribution
  cat("  loss distribution: ")
  obs <- tabulate(portfolio[,1]+1, 101);
  prob <- dbinom(0:100, 100, 0.1);
  cst <- chisq.test(obs, p=prob);
  if (cst$p.value > 0.1) { 
    cat("OK\n");
  } else { 
    cat("FAILED\n"); 
  }

}

#===========================================================================
# test04
#===========================================================================
test04 <- function()
{
  cat("test04\n");
  
  #reading data
  portfolio <- ccruncher.read("data/test04/portfolio.csv");

  #checking EL
  cat("  expected loss: ")
  risk <- ccruncher.risk(portfolio[,1])
  if (100 < risk$mean-qnorm(0.95)*risk$mean_stderr | risk$mean+qnorm(0.95)*risk$mean_stderr < 100) {
    cat("FAILED\n");
  } else {
    cat("OK\n");
  }
  
  # cumulative distribution function (cdf)
  pf <- function(x, p, w) {
    ifelse(x <=0|x>=1, 0, pnorm((qnorm(x)*sqrt(1-w^2)-qnorm(p))/w))
  }
  
  #check portfolio loss distribution
  cat("  portfolio loss distribution: ")
  numobligors = 1000
  kstest = ks.test(portfolio[1:1000,1]/numobligors, pf, p=0.1, w=0.2)
  if (kstest$p.value < 0.1) {
    cat("FAILED\n");
  } else {
    cat("OK\n");
  }

  #checking VAR
  cat("  value at risk: ")
  p = 0.1
  w = 0.2
  percentiles=risk$VAR[1:4,1]
  evars = pnorm((qnorm(p)+w*qnorm(percentiles))/sqrt(1-w^2))
  aux = abs(risk$VAR[1:4,2] - evars*1000);
  if (length(aux[aux>=10])) {
    cat("FAILED\n");
  } else { 
    cat("OK\n"); 
  }
  
}

#===========================================================================
# test05
#===========================================================================
test05 <- function()
{
  cat("test05\n");
  
  #reading data
  portfolio <- ccruncher.read("data/test05/portfolio.csv");
  secrat <- ccruncher.read("data/test05/sector-rating.csv");

  #checking segmention completeness
  cat("  segmentations consistency: ")
  if (isComplete(portfolio, secrat)) {
    cat("OK\n");
  } else {
    cat("FAILED\n"); 
  }
  
}

#===========================================================================
# test06
#===========================================================================
test06 <- function()
{
  cat("test06\n");
  
  #reading data
  portfolio <- ccruncher.read("data/test06/portfolio.csv");
  obligors <- ccruncher.read("data/test06/obligors.csv");
  assets <- ccruncher.read("data/test06/assets.csv");
  n = nrow(portfolio)

  # checking aggregation
  defaulted = (obligors[,]!=0)
  aux = matrix(nrow=n, ncol=4, 0)
  aux[,1] = defaulted[,1]*10 + defaulted[,2]*100
  aux[,2] = defaulted[,1]*20 + defaulted[,2]*200
  aux[,3] = defaulted[,1]*30 + defaulted[,2]*300
  aux[,4] = defaulted[,3]*6
  
  cat("  segments values: ")
  if (sum(obligors[,1]!=0 & obligors[,1]!= 60) != 0) {
    cat("FAILED\n"); 
  } else if (sum(obligors[,2]!=0 & obligors[,2]!= 600) != 0) {
    cat("FAILED\n"); 
  } else if (sum(obligors[,3]!=0 & obligors[,3]!= 6) != 0) {
    cat("FAILED\n"); 
  } else if (sum(aux != assets) != 0) {
    cat("FAILED\n"); 
  } else {
    cat("OK\n"); 
  }
  
}

