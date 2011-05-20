
#***************************************************************************
#
# CreditCruncher - A portfolio credit risk valorator
# Copyright (C) 2004-2011 Gerard Torrent
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


#ccruncher.bincor <- function(a, c)
#{
#  ret <- vector(length=3);
#  ret[2] <- (1-c)*((1-a)*a^2+a*(1-a)^2);
#  ret[1] <- (1-a)-ret[2];
#  ret[3] <- a-ret[2];
#  ret[2] <- 2*ret[2];
#  return(ret);
#}

#===========================================================================
# description
#   given a sector correlation matrix creates the obligor matrix
# arguments
#   smatrix: matrix. sectorial correlation matrix
#   bpsector: array. number of obligors per sector
# returns
#   obligor correlation matrix
# example
#   smatrix <- matrix(nrow=3,ncol=3,c(0.2,0.05,0.03,0.05,0.25,0.07,0.03,0.07,0.4))
#   bpsector <- c(3,2,4)
#   s2bcorrel(smatrix, bpsector)
#   smatrix <- matrix(nrow=2,ncol=2,c(0.2,0.1,0.1,0.25))
#   bpsector <- c(44,56)
#   s2bcorrel(smatrix, bpsector)
#===========================================================================
s2bcorrel <- function(smatrix, bpsector)
{
  numobligors <- sum(bpsector)
  ret <- matrix(nrow=0, ncol=numobligors)

  for(i in 1:length(bpsector)) {
    dimx <- bpsector[i]
    aux <- matrix(nrow=dimx, ncol=0)
    for(j in 1:length(bpsector)) {
      dimy <- bpsector[j]
      val <- smatrix[i,j]
      M <- matrix(nrow=dimx, ncol=dimy, smatrix[i,j])
      aux <- cbind(aux, M)
    }
    ret <- rbind(ret, aux)
  }

  for(i in 1:numobligors) {
    ret[i,i] = 1
  }

  return(ret)
}

#===========================================================================
# description
#   checks that the given copula values satisfies the given correlation
# arguments
#   copula: copula data frame
#   correlations: expected correlations matrix
# returns
#   TRUE=ok, FALSE=otherwise
# example
#   checkCorrelations(copula, correlations)
#===========================================================================
checkCorrelations <- function(copula, ecorrels)
{
  ocorrels <- cor(copula)
  for(i in 1:length(copula)) {
    for(j in 1:i) {
      if (i!=j && abs(ecorrels[i,j]-ocorrels[i,j]) > 0.05) { 
        return(FALSE);
      }
    }
  }
  return(TRUE);
}

#===========================================================================
# description
#   checks if each copula component is an Uniform[0,1] distribution
# arguments
#   copula: copula data frame
# returns
#   TRUE=ok, FALSE=otherwise
# example
#   areUniforms(copula)
#===========================================================================
areUniforms <- function(copula)
{
  n = length(copula)
  ret <- vector(length=n)
  for(i in 1:n) {
    ret[i] = isUniform(copula[[i]])
  }
  fails = length(ret[ret==FALSE]);
  return(fails/n <= 0.05)
}

#===========================================================================
# description
#   checks if an array of values have a Uniform[0,1] distribution
# arguments
#   data: array of values
# returns
#   TRUE=ok, FALSE=otherwise
# example
#   isUniform(data)
#===========================================================================
isUniform <- function(data)
{
  if (length(data[data < 0]) > 0 || length(data[data > 1]) > 0) {
    return(FALSE);
  }

  #does the Kolmogorov-Smirnof test
  kstest <- ks.test(unique(data), "punif")
  if (kstest$p.value > 0.05) {
    return(TRUE);
  }

  #if fails, we try chi-square test
  numbreaks <- 10
  obs <- table(cut(data,(1/numbreaks)*(0:numbreaks)))
  prob <- obs
  prob[] <- 1/numbreaks;
  cstest <- chisq.test(obs, p=prob);
  if (cstest$p.value > 0.05) {
    return(TRUE);
  }

  #otherwise, this is not a uniform
  return(FALSE);
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
cat(aux[aux>epsilon])
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
  copula <- ccruncher.read("data/test01/copula.csv");

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
  if (cst$p.value > 0.01) { 
    cat("OK\n");
  } else { 
    cat("FAILED\n"); 
  }

  #checking copula (uniform)
  cat("  copula distributions: ")
  if (areUniforms(copula)) {
    cat("OK\n");
  } else { 
    cat("FAILED\n"); 
  }
  
  #checking copula (correlations)
  cat("  copula correlations: ")
  correlations = matrix(nrow=1,ncol=1,c(1.0))
  if (checkCorrelations(copula, correlations)) {
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
  copula <- ccruncher.read("data/test02/copula.csv");

  #checking loss distribution
  cat("  loss distribution: ")
  obs <- tabulate(portfolio[,1]+1, 3);
  prob <- c(0.9*0.9, 0.9*0.1+0.1*0.9, 0.1*0.1);
  cst <- chisq.test(obs, p=prob)
  if (cst$p.value > 0.01) { 
    cat("OK\n");
  } else { 
    cat("FAILED\n"); 
  }

  #checking copula (uniform)
  cat("  copula distributions: ")
  if (areUniforms(copula)) {
    cat("OK\n");
  } else { 
    cat("FAILED\n"); 
  }
  
  #checking copula (correlations)
  cat("  copula correlations: ")
  correlations = diag(2)
  if (checkCorrelations(copula, correlations)) {
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
  copula <- ccruncher.read("data/test03/copula.csv");

  #checking loss distribution
  cat("  loss distribution: ")
  obs <- tabulate(portfolio[,1]+1, 101);
  prob <- dbinom(0:100, 100, 0.1);
  cst <- chisq.test(obs, p=prob);
  if (cst$p.value > 0.01) { 
    cat("OK\n");
  } else { 
    cat("FAILED\n"); 
  }

  #checking copula (uniform)
  cat("  copula distributions: ")
  if (areUniforms(copula)) {
    cat("OK\n");
  } else { 
    cat("FAILED\n"); 
  }
  
  #checking copula (correlations)
  cat("  copula correlations: ")
  correlations = diag(100)
  if (checkCorrelations(copula, correlations)) {
    cat("OK\n");
  } else { 
    cat("FAILED\n"); 
  }

}

#===========================================================================
# description
#   checks that every column is a uniform[0,1]
# arguments
#   filename: string. segmentation filename
# returns
#   none
# example
#   test4("copula.csv", correls)
#===========================================================================
test04 <- function()
{
  cat("test04\n");
  
  #reading data
  portfolio <- ccruncher.read("data/test04/portfolio.csv");
  sectors <- ccruncher.read("data/test04/sectors.csv");
  copula <- ccruncher.read("data/test04/copula.csv");

  #checking segmention completeness
  cat("  segmentations consistency: ")
  if (isComplete(portfolio, sectors)) {
    cat("OK\n");
  } else {
    cat("FAILED\n"); 
  }

  #checking copula (uniform)
  cat("  copula distributions: ")
  if (areUniforms(copula)) {
    cat("OK\n");
  } else { 
    cat("FAILED\n"); 
  }
  
  #checking copula (correlations)
  cat("  copula correlations: ")
  smatrix <- matrix(nrow=2,ncol=2,c(0.10,0.05,0.05,0.15))
  bpsector <- c(50,50)
  correlations = s2bcorrel(smatrix, bpsector)
  if (checkCorrelations(copula, correlations)) {
    cat("OK\n");
  } else { 
    cat("FAILED\n"); 
  }
  
}

