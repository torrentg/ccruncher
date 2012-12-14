
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
  if (cst$p.value > 0.01) { 
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
  if (cst$p.value > 0.01) { 
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
  if (cst$p.value > 0.01) { 
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
  
  #check portfolio loss distribution
  cat("  portfolio loss distribution: ")
  pf <- function(x, p, w) {
    ifelse(x <= 0, 0, pnorm((qnorm(x)*sqrt(1-w^2)-qnorm(p))/w))
  }
  kstest = ks.test(portfolio[1:1000,1]/1000, pf, p=0.1, w=0.2)
  if (kstest$p.value < 0.05) {
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
  sectors <- ccruncher.read("data/test05/sectors.csv");

  #checking segmention completeness
  cat("  segmentations consistency: ")
  if (isComplete(portfolio, sectors)) {
    cat("OK\n");
  } else {
    cat("FAILED\n"); 
  }
  
}
