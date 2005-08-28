
#=========================================================
# this file is a R script (see http://www.r-project.org/)
# move to data where output data files are located
#   cd $CCRUCNHER/data/
# execute R
#   R
# in the R console type:
#   source("report.R")
#   ccruncher.main("portfolio-rest.out", var=0.99, alpha=0.975, show=2)
#   quit(save='no')
#=========================================================

#=========================================================
# Utility function (concatenate 2 strings)
#=========================================================
"%&%" <- function(a,b) paste(a,b,sep="")

#=========================================================
# Computes the standar error for prob-quantile using
# Maritz-Jarrett method
# example: VAR(99.9) => quantstderr(x,0.01)
#=========================================================
ccruncher.quantstderr <- function(x,prob,sorted=FALSE)
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
    if (W[i] < 1e-20) break;
  }
  for(i in M:1)
  {
    W[i] <- pbeta((i+1)/N,A,B) - pbeta(i/N,A,B);
    if (W[i] < 1e-20) break;
  }

  #computing C1 and C2
  C1 <- sum(W*y);
  C2 <- sum(W*y*y);

  #returning quantile standar error
  return(sqrt(C2-C1^2));
}

#=========================================================
# Given a vector of length n, returns a 2xn-matrix
# row1: mean
# row2: standar error
#=========================================================
ccruncher.evalmean <- function(x)
{
  #initializing values
  ret <- matrix(NaN,2,length(x));
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
    sigma <- sqrt((aux2-aux1*aux1/i)/(i-1));

    ret[1,i] <- mu;
    ret[2,i] <- sigma/sqrt(i);
  }

  #returning values
  return(ret);
}

#=========================================================
# Given a vector of length n, returns a 2xn-matrix
# row1: std dev
# row2: standar error
#=========================================================
ccruncher.evalsigma <- function(x)
{
  #initializing values
  ret <- matrix(NaN,2,length(x));
  ret[1,1] <- 0;
  ret[2,1] <- 0;
  aux1 <- x[1];
  aux2 <- x[1]*x[1];

  #computing values
  for(i in 2:length(x))
  {
    aux1 <- aux1 + x[i];
    aux2 <- aux2 + x[i]*x[i];

    sigma <- sqrt((aux2-aux1*aux1/i)/(i-1));

    ret[1,i] <- sigma;
    ret[2,i] <- sigma/sqrt(2*i);
  }

  #returning values
  return(ret);
}

#=========================================================
# Given a vector of length n, returns a 2xn-matrix
# row1: quantile
# row2: standar error
# ret3 <- ccruncher.evalquantile(z,0.01)
#=========================================================
ccruncher.evalquantile <- function(x,prob,breaks=250)
{
  #initializing values
  ret <- matrix(NA,2,length(x));
  ret[1,1] <- 0;
  ret[2,1] <- 0;
  aux <- vector();
  aux[1] <- x[1];
  k <- max(1, as.integer(trunc(length(x)/breaks)));

  #computing values
  for(i in 2:length(x))
  {
    aux[i] <- x[i];

    if (i%%k == 0 | i >= length(x)-10)
    {
      aux <- sort(aux,method="sh");

      ret[1,i] <- quantile(aux[1:i],prob,names=FALSE);
      ret[2,i] <- ccruncher.quantstderr(aux[1:i],prob,sorted=TRUE);
    }
    else
    {
      #ret[1,i] <- quantile(aux[1:i],prob,names=FALSE);
      ret[1,i] <- ret[1,i-1];
      ret[2,i] <- ret[2,i-1];
    }
  }

  #returning values
  return(ret);
}

#=========================================================
# Given a 2xn-matrix containing statistic & standar error
# plots a graphics
# example: plotevol(x,0.99)
#=========================================================
ccruncher.plotevol <- function(values, alpha, statname="statistic")
{
  #retrieving length
  n <- length(values[1,]);

  #computing confidence level
  k <- qnorm((1-alpha)/2);

  #finding a valid yrange
  yrange <- vector(length=2);

  yrange[1] <- values[1,n] - 7.5*abs(k*values[2,n]);
  yrange[2] <- values[1,n] + 7.5*abs(k*values[2,n]);

  #plotting statistic evolution
  plot(values[1,], type='l', ylim=yrange,
       main=statname%&%" convergence",
       xlab="Monte Carlo iteration",
       ylab=statname%&%" + "%&%(alpha*100)%&%"% confidence bound");
  par(new=TRUE);

  #plotting confidence levels bounds
  lines(values[1,]+k*values[2,], lty=3);
  lines(values[1,]-k*values[2,], lty=3);

  #plotting last statistic value
  abline(values[1,length(values[1,])],0);
}

#=========================================================
# Compute the main statistics
#=========================================================
ccruncher.summary <- function(x,var,alpha)
{
  n <- length(x);
  mu <- mean(x);
  sigma <- sqrt(var(x));
  q <- quantile(x, 1-var, names=FALSE);
  qse <- ccruncher.quantstderr(x, 1-var);
  k <- qnorm((1-alpha)/2);

  print("", quote=FALSE);
  print("summary at "%&%(alpha)%&%" confidence level", quote=FALSE);
  print("-------------------------------------------------------------------------", quote=FALSE);
  print("expected value = " %&% mu %&% " +/- " %&% abs(k*sigma/sqrt(n)), quote=FALSE);
  print("standard deviation = " %&% sigma %&% " +/- " %&% abs(k*sigma/sqrt(2*n)), quote=FALSE);
  print("VAR(" %&% var %&% ") = " %&% q %&% " +/- " %&% abs(k*qse), quote=FALSE);
  print("", quote=FALSE);
}

#=========================================================
# VaR table
#=========================================================
ccruncher.vartable <- function(x)
{
  xvar <- c(0.90, 0.95, 0.975, 0.99, 0.995, 0.999, 0.9999);
  vvar <- vector(length=length(xvar));
  svar <- vector(length=length(xvar));

  for(i in 1:length(xvar))
  {
    vvar[i] <- quantile(x, (1-xvar[i]));
    svar[i] <- ccruncher.quantstderr(x, (1-xvar[i]));
  }

  return(data.frame(VaR=xvar,value=vvar,stderr=svar))
}

#=========================================================
# main function
# show=1 -> results + density graphic + convergence graphics
# show=2 -> results + density graphic
# show=3 -> results
#=========================================================
ccruncher.main <- function(filename, var=0.99, alpha=0.99, show=2)
{
  #checking arguments
  if (show < 1 | 3 < show)
  {
    return("invalid show value. choose one of 1/2/3");
  }

  #retrieving data file
  df <- read.table(filename, col.names=c('index','value'));
  z <- as.vector(t(df["value"]));

  if (show == 1)
  {
    #computing statistics evolution values
    ret1 <- ccruncher.evalmean(z);
    ret2 <- ccruncher.evalsigma(z);
    ret3 <- ccruncher.evalquantile(z,prob=(1-var),breaks=250);

    #doing some graphics
    par(mfrow=c(2,2));
    plot(density(z));
    ccruncher.plotevol(ret1,alpha,"Mean");
    ccruncher.plotevol(ret2,alpha,"StdDev");
    ccruncher.plotevol(ret3,alpha,"VaR("%&%(var*100)%&%"%)");
  }
  else if (show == 2)
  {
    #plotting density graph
    plot(density(z));
  }

  #print results
  print("", quote=FALSE);
  ccruncher.summary(z,var=var,alpha=alpha);
  print("", quote=FALSE);
  print("VaR table", quote=FALSE);
  print("-------------------------------------------------------------------------", quote=FALSE);
  ccruncher.vartable(z);
}
