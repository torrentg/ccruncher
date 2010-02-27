ccruncher.bincor <- function(a, c)
{
  ret <- vector(length=3);
  ret[2] <- (1-c)*((1-a)*a^2+a*(1-a)^2);
  ret[1] <- (1-a)-ret[2];
  ret[3] <- a-ret[2];
  ret[2] <- 2*ret[2];
  return(ret);
}

#read copula simulated values
df <- read.table("borrame.txt", col.names=c('x1', 'x2'));
#computing correlations between copula variables
cor(df$x1,df$x2)

#reading results
x <- ccruncher.read("data/portfolio.out")
#computing observed probabilities
tabulate(x+1, 3)/200000
#computing theoretical probabilities
ccruncher.bincor(0.1, 0.25)

#checks that all segments of a segmentation sums the total
tsegmentation <- t(read.table("sector.csv", sep=","));
#sums by segment
apply(tsegmentation,1,sum)
#sums by simulation
apply(tsegmentation,2,sum)

#check that only exists 2 segments
length(tsegmentation) = 2
sum(tsegmentation[1]) > 0
sum(tsegmentation[2]) > 0
tportfolio <- read.table("portfolio.csv", sep=",");
aux <- tportfolio[1] - tsegmentation[1] - tsegmentation[2]
aux[abs(aux) > 0.010000001]
sum(tportfolio[1] - tsegmentation[1] - tsegmentation[2]) < 20000*0.01

ks.test(x, "punif") >> GTG: acabar de mirar
#numeros repetits
t(copula[1])[which(duplicated(copula[1]))]
unique()

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
#   checks that 2 segmentations sums the same
# arguments
#   filename1: string. segmentation1 filename
#   filename2: string. segmentation2 filename
# returns
#   none
# example
#   test1("portfolio.csv", "sector.csv")
#===========================================================================
test1 <- function(filename1, filename2)
{
  #reading data
  segmentation1 <- read.csv(filename1, header=TRUE, sep=",");
  segmentation2 <- read.csv(filename2, header=TRUE, sep=",");

  #for each simulation: sums(segments1) - sum(segments2)
  aux <- apply(segmentation1,1,sum) - apply(segmentation2,1,sum)

  #can be some minor diference due to rounding
  epsilon <- 0.01*max(length(segmentation1), length(segmentation2)) + 0.00001
  if (length(aux[aux>epsilon])) { 
    cat("test1 failed\n");
  }
  else {
    cat("test1 passed\n");
  }
}

#===========================================================================
# description
#   checks if segments with no values are correct
# arguments
#   filename: string. segmentation filename
#   hasvalues: vector. TRUE=hasvalue, false=otherwise
# returns
#   none
# example
#   test2("sector.csv", c(TRUE, FALSE, TRUE, TRUE))
#   test2("asset.csv", rep(TRUE,times=400))
#===========================================================================
test2 <- function(filename, hasvalues)
{
  #reading data
  segmentation <- read.csv(filename, header=TRUE, sep=",");

  #checking sizes
  if (length(segmentation) != length(hasvalues)) {
    return(cat("test2 failed\n"));
  }

  #summing all values by segment
  aux <- apply(segmentation,2,sum)
  epsilon <- 0.0001

  #checking asumptions
  for(i in length(hasvalues)) {
    if(hasvalues[i] == TRUE && abs(aux[i]) < epsilon) {
      return(cat("test2 failed\n"));
    }
    if(hasvalues[i] == FALSE && abs(aux[i]) > epsilon) {
      return(cat("test2 failed\n"));
    }    
  }
  cat("test2 passed\n");
}

#===========================================================================
# isUniform
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

  #else, this is not a uniform
  return(FALSE);
}

#===========================================================================
# description
#   checks that every column is a uniform[0,1]
# arguments
#   filename: string. segmentation filename
# returns
#   none
# example
#   test3("sector.csv")
#===========================================================================
test3 <- function(filename)
{
  #reading data
  copula <- read.csv(filename, header=TRUE, sep=",");

  ret <- array(dim=length(copula));

  for(i in 1:length(copula)) {
    if (!isUniform(copula[[i]])) {
      cat("test3 failed for " %&% names(copula)[i] %&% "\n");
    }
  }
  return(cat("test3 passed\n"));
}


#===========================================================================
# description
#   given a sector correlation matrix creates the borrower matrix
# arguments
#   smatrix: matrix. sectorial correlation matrix
#   bpsector: array. number of borrowers per sector
# returns
#   borrower correlation matrix
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
  numborrowers <- sum(bpsector)
  ret <- matrix(nrow=0, ncol=numborrowers)

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

  for(i in 1:numborrowers) {
    ret[i,i] = 1
  }

  return(ret)
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
test4 <- function(filename, matrix)
{
  #reading data
  copula <- read.csv(filename, header=TRUE, sep=",");

  ret <- array(dim=length(copula));

  for(i in 1:length(copula)) {
    for(j in (i+1):length(copula)) {
      if (!isUniform(copula[[i]])) {
        correl <- cor(copula[[i]], copula[[j]])
        if (abs(correl-matrix[i,j]) > 0.01) {
          cat("correl[" %&% i %&% "," %&% j %&% "] broken " %&% correl %&% "\n")
        }
      }
    }
  }
}

