
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
#   bivariate t-student spearman's rank approximation and its inverse
# arguments
#   rho: linear/pearson correlation
#   ndf: t-student degrees of freedom
# returns
#   bivariate spearman's rank
# example
#   ccruncher.g(0.3, 15)
#   ccruncher.ginv(0.3, 15)
#===========================================================================
ccruncher.h <- function(ndf)
{
  ret = pi/6 + 1/(0.44593 + 1.3089*ndf)
  return(ret);
}

ccruncher.g <- function(rho, ndf)
{
  ret = asin(rho*sin(ccruncher.h(ndf)))/ccruncher.h(ndf)
  return(ret);
}

ccruncher.ginv <- function(rho, ndf)
{
  ret = sin(rho*ccruncher.h(ndf))/sin(ccruncher.h(ndf))
  return(ret);
}

#===========================================================================
# description
#   create a dense block matrix
# arguments
#   sigma: square matrix of dim KxK, where K
#          is the number of sectors. sigma(i,j) is
#          the correlation between an i-company and an 
#          j-company, where 1=<i,j<=K.
#   dims: vector of dim K, where K is the number of 
#          sectors. dims(i) is the number of components 
#          in sector i.
# returns
#   a dense matrix of size NxN, where N = sum(dims)
# example
#   sigma = matrix(nrow=3, ncol=3, data=c(0.5,0.2,0.1,0.2,0.4,0.15,0.1,0.15,0.3))
#   dims = c(3,2,1)
#   ccruncher.cdbm(sigma, dims)
#===========================================================================
ccruncher.cdbm <- function(sigma, dims)
{
  # check parameter sigma
  if (!is.matrix(sigma) | dim(sigma)[1] != dim(sigma)[2]) 
    stop("invalid sigma matrix");
  K = dim(sigma)[1]
  for (i in 1:K) {
    for (j in i:K) {
      if (sigma[i,j] != sigma[j,i]) 
        stop("non-simmetric sigma matrix");
    }
  }
  
  # check parameter dims
  if (length(dims) != K) 
    stop("matrix size distinct than dims size");
  if (length(dims[dims<0]) > 0) 
    stop("dims contains negative values");
  N = sum(dims)
  if (N == 0) 
    stop("all dims components are 0");

  # creating return matrix
  ret <- matrix(0, nrow=N, ncol=N);

  # filling block matrix
  r1 = 1;
  for (i in 1:K) 
  {
    if (dims[i] > 0)
    {
      r2 = sum(dims[1:i]);
      c1 = r1;
      for (j in i:K) 
      {
        if (dims[j] > 0)
        {
          c2 = sum(dims[1:j]);
          ret[r1:r2,c1:c2] = sigma[i,j];
          ret[c1:c2,r1:r2] = sigma[i,j];
          c1 = c2+1;
        }
      }
      r1 = r2+1;
    }
  }
  
  # setting diagonal (=1)
  diag(ret) = 1;
  
  return(ret);
}

#===========================================================================
# description
#   obtain eigen values and eigenvectors for a given block matrix
# arguments
#   sigma: square matrix of dim KxK, where K
#          is the number of sectors. sigma(i,j) is
#          the correlation between an i-company and an 
#          j-company, where 1=<i,j<=K.
#   dims: vector of dim K, where K is the number of 
#          sectors. dims(i) is the number of components 
#          in sector i.
# returns
#   $G deflated matrix
#   $values eigenvalues
#   $multiplicity eigenvalues multiplicity
# example
#   sigma = matrix(nrow=3, ncol=3, data=c(1,2,3,2,4,5,3,5,6))
#   dims = c(2,3,1)
#   ccruncher.eigen(sigma, dims)
#===========================================================================
ccruncher.eigen <- function(sigma, dims)
{
  # check parameter sigma
  if (!is.matrix(sigma) | dim(sigma)[1] != dim(sigma)[2]) 
    stop("invalid sigma matrix");
  K = dim(sigma)[1]
  for (i in 1:K) {
    for (j in i:K) {
      if (sigma[i,j] != sigma[j,i]) 
        stop("non-simmetric sigma matrix");
    }
  }
  
  # check parameter dims
  if (length(dims) != K) 
    stop("matrix size distinct than dims size");
  if (length(dims[dims<0]) > 0) 
    stop("dims contains negative values");
  N = sum(dims)
  if (N == 0) 
    stop("all dims components are 0");

  # creating deflated matrix
  G <- matrix(0, nrow=K, ncol=K)
  for (i in 1:K)
  {
    for(j in 1:K)
    {
      if (i == j) G[i,i] = 1+(dims[j]-1)*sigma[i,i]
      else G[i,j] = dims[j]*sigma[i,j]
    }
  }
  
  # computing G eigenvalues + eigenvectors
  eig=eigen(G)
  
  num = K + length(dims[dims>1])
  values = vector(length=num)
  multiplicity = vector(length=num)

  s = 1
  for(i in 1:K)
  {
    values[s] = eig$values[i]
    multiplicity[s] = 1
    s = s + 1
    if (dims[i] > 1)
    {
      values[s] = 1 - sigma[i,i]
      multiplicity[s] = dims[i]-1
      s = s + 1
    }
  }
  
  return(list(G=G,values=values,multiplicity=multiplicity))
}

#===========================================================================
# description
#   correlation matrix operator
# arguments
#   A: square matrix
# returns
#   square matrix B, where diag(B)=1 and Bij = Aij/sqrt(Aii*Ajj)
# example
#   sigma = matrix(nrow=3, ncol=3, data=c(1,2,3,2,4,5,3,5,6))
#   ccruncher.cmo(sigma)
#===========================================================================
ccruncher.cmo <- function(A)
{
  k = dim(A)[1]
  
  for(i in 1:k)
  {
    for(j in 1:k)
    {
      if (i != j)
      {
        A[i,j] = A[i,j]/(sqrt(A[i,i])*sqrt(A[j,j]))
      }
    }
  }
  diag(A) = 1
  return(A)
}

#===========================================================================
# description
#   Cholesky algorithm for block matrix
#   see 'Simulation of High-Dimensional t-Student Copulas'
#   by Gerard Torrent-Gironella and Josep Fortiana
# arguments
#   M: matrix of block values (size=kxk)
#   n: vector of elements per block (length=k)
# returns
#   k: number of blocks
#   n: element per block
#   M: block values
#   H: Cholesky values
#   v: Cholesky values
# example
#   sigma = matrix(nrow=3, ncol=3, data=c(0.5,0.2,0.1,0.2,0.4,0.15,0.1,0.15,0.3))
#   dims = c(3,2,1)
#   ccruncher.chold(sigma,dims)
#===========================================================================
ccruncher.chold <- function(M, n)
{
  #TODO: check parameters

  k = dim(M)[1]
  size = sum(n);
    
  H = matrix(nrow=k, ncol=size, 0)
  v = vector(length=size)
    
  v[1] = 1.0;

  for(r in 1:k)
  {
    H[r,1] = M[r,1]/v[1];
  }

  val1 = 0
  i = 0
  for(r in 1:k)
  {
    for(q in 1:n[r])
    {
      i = i+1
      if (i != 1)
      {
        if (q == 1) {
          val1 = sum(H[r,1:(i-1)]^2)
        }
        else {
          val1 = val1 + H[r,i-1]^2
        }
        if (1-val1 < 0.0) {
          stop("non definite-positive matrix");
        }
        v[i] = sqrt(1-val1)
        for(s in r:k)
        {
          if (s!=r | q!= n[r])
          {
            H[s,i] = (M[s,r]-sum(H[r,1:(i-1)]*H[s,1:(i-1)]))/v[i]
          }
        }
      }
    }
  }

  return(list(n=n, M=M, k=k, H=H, v=v));
}

#===========================================================================
# description
#   Multiplies a Cholesky matrix by a vector
#   see 'Simulation of High-Dimensional t-Student Copulas'
#   by Gerard Torrent-Gironella and Josep Fortiana
# arguments
#   O: Cholesky decomposition object
#   x: vector (length=sum(O$n))
# returns
#   y: chol()%*%x
# example
#   sigma = matrix(nrow=3, ncol=3, data=c(0.5,0.2,0.1,0.2,0.4,0.15,0.1,0.15,0.3))
#   dims = c(3,2,1)
#   O = ccruncher.chold(sigma,dims)
#   ccruncher.mult(O,1:6)
#===========================================================================
ccruncher.mult <- function(O, x)
{
  k = O$k
  size = sum(O$n)

  y = vector(length=size)
  y[] = 0

  i = 0
  for(r in 1:k)
  {
    i = i+1
    if (i > 1) {
      y[i] = sum(O$H[r,1:(i-1)]*x[1:(i-1)])
    }

    if (O$n[r] > 1)
    {
      for(q in 1:(O$n[r]-1))
      {
        i = i+1
        y[i] = y[i-1] + O$H[r,i-1]*x[i-1]
      }
    }
  }

  for(j in 1:i)
  {
    y[j] = y[j] + O$v[j]*x[j]
  }
    
  return(y)
}

#===========================================================================
# description
#   returns n realizations of the t-student block copula
#   see 'Simulation of High-Dimensional t-Student Copulas'
#   by Gerard Torrent-Gironella and Josep Fortiana
# arguments
#   O: Cholesky decomposition object
#   nu: degrees of freedom
#   n: number of realizations
# returns
#   matrix (nrow=n, ncol=sum(O$n))
# example
#   sigma = matrix(nrow=3, ncol=3, data=c(0.5,0.2,0.1,0.2,0.4,0.15,0.1,0.15,0.3))
#   dims = c(3,2,1)
#   sigma1 = ccruncher.ginv(sigma,15)
#   O = ccruncher.chold(sigma1,dims)
#   X = ccruncher.next(O,15,100000)
#   cor(X)
#===========================================================================
ccruncher.next <- function(O, nu, n=1)
{
  size = sum(O$n)
  ret = matrix(nrow=n, ncol=size)

  for (i in 1:n)
  {
    ret[i,] = ccruncher.mult(O,rnorm(size))
    s = rchisq(1,nu)
    ret[i,] = sqrt(nu/s)*ret[i,]
    ret[i,] = pt(ret[i,],nu)
  }
    
  return(ret)
}

#===========================================================================
# description
#   simulate t-student copula using factor model algorithm
# arguments
#   sigma: correlation matrix (block matrix form)
#   dims: numbers of components by sector
#   nu: degrees of freedom
#   n: number of realizations
# returns
#   matrix (nrow=n, ncol=sum(O$n))
# example
#   sigma = matrix(nrow=3, ncol=3, data=c(0.5,0.2,0.1,0.2,0.4,0.15,0.1,0.15,0.3))
#   dims = c(3,2,1)
#   sigma1 = ccruncher.ginv(sigma,15)
#   X = ccruncher.fmodsim(sigma1,dims,15,100000)
#   cor(X)
#===========================================================================
ccruncher.fmodsim <- function(sigma, dims, nu, n=1)
{
  k = dim(sigma)[1]
  w = sqrt(diag(sigma))
  M = ccruncher.cmo(sigma)
  R = t(chol(M))

  size = sum(dims)
  ret = matrix(nrow=n, ncol=size)

  for (row in 1:n)
  {
    z = R %*% rnorm(k)
    
    col = 1
    for (i1 in 1:k)
    {
      for (i2 in 1:dims[i1])
      {
        ret[row,col] = w[i1]*z[i1] + sqrt(1-w[i1]^2)*rnorm(1)
        col = col+1
      }
    }
    
    s = rchisq(1,nu)
    ret[row,] = sqrt(nu/s)*ret[row,]
    ret[row,] = pt(ret[row,],nu)
  }
    
  return(ret)
}

#===========================================================================
# description
#   simulate default counts per sector
# arguments
#   O: Cholesky decomposition object
#   nu: degrees of freedom
#   p: yearly default probabilities (length=sum(O$n))
#   n: number of realizations
# returns
#   matrix (nrow=n, ncol=O$k)
# example
#   sigma = matrix(nrow=3, ncol=3, data=c(0.5,0.2,0.1,0.2,0.4,0.15,0.1,0.15,0.3))
#   dims = c(3,2,1)
#   O = ccruncher.chold(sigma,dims)
#   ccruncher.simcounts(O,15,c(0.03,0.04,0.05),1000)
#===========================================================================
ccruncher.simcounts <- function(O, nu, p, n=1)
{
  size = sum(O$n)
  ret = matrix(0, nrow=n, ncol=O$k)

  for(i in 1:n)
  {
    # TODO: replace next by fmodsim
    x = ccruncher.next(O,nu)
    
    i1 = 0
    for(j in 1:O$k)
    {
      i2 = i1 + O$n[j]
      i1 = i1 + 1
      ret[i,j] = sum(x[i1:i2]<p[j])
      i1 = i2
    }
  }
  
  return(ret)
}

#===========================================================================
# description
#   adapted eigenvalues method
#   see 'Simulation of High-Dimensional t-Student Copulas'
#   by Gerard Torrent-Gironella and Josep Fortiana
# arguments
#   sigma: correlation matrix (size=kxk)
#   dims: elements per sector (length=k)
#   epsilon: minimum eigenvalue value
# returns
#   coerced correlation matrix
# example
#   sigma = matrix(nrow=3, ncol=3, data=c(0.5,0.99,0.1,0.99,0.4,0.15,0.1,0.15,0.3))
#   dims = c(3,2,1)
#   ccruncher.coerce(sigma,dims)
#===========================================================================
ccruncher.coerce <- function(sigma, dims, epsilon=0.05)
{
  k = length(dims)
  G1 = ccruncher.eigen(sigma, dims)$G
  eig1 = eigen(G1)
  eig1$values[eig1$values<=0] = epsilon
  G2 = eig1$vectors %*% diag(eig1$values) %*% solve(eig1$vectors)
  epsilons = 1+(diag(G2)-diag(G1))/dims
  for(i in 1:k) 
  {
    for(j in 1:k) 
    {
      G2[i,j] = G2[i,j]/(sqrt(epsilons[i])*sqrt(epsilons[j]))
    }
  }
  aux = diag(G2)
  sigma2 = t(t(G2)/dims)
  diag(sigma2) = (aux-1)/(dims-1)
  sigma2[!is.finite(sigma2)] = 1
  sigma2 = (sigma2+t(sigma2))/2
  return(sigma2)
}

#===========================================================================
# description
#   inverse Cholesky algorithm for block matrix
#   see 'Simulation of High-Dimensional t-Student Copulas'
#   by Gerard Torrent-Gironella and Josep Fortiana
# arguments
#   O: cholesky decomposition object
# returns
#   k: number of blocks
#   n: element per block
#   J: inverse Cholesky values
#   w: inverse Cholesky values
# example
#   sigma = matrix(nrow=3, ncol=3, data=c(0.5,0.2,0.1,0.2,0.4,0.15,0.1,0.15,0.3))
#   dims = c(3,2,1)
#   O = ccruncher.chold(sigma,dims)
#   ccruncher.choldinv(O)
#===========================================================================
ccruncher.choldinv <- function(O)
{
  #TODO: check parameters

  k = O$k;
  size = sum(O$n);
    
  J = matrix(nrow=size, ncol=k, 0)
  w = vector(length=size)
    
  for(i in 1:size) 
  { 
    w[i] = 1.0/O$v[i];
  }

  i = 1
  for(r in 1:k)
  {
    j = i-1
    for(s in r:k)
    {
      sum = -O$H[s,i]*w[i];
      if (i+1 <= j-1) {
        sum = sum - sum(O$H[s,(i+1):(j-1)]*J[(i+1):(j-1),r]);
      }
      for(q in 1:O$n[s])
      {
        j = j+1
        if (j!=i | s!=r)
        {
          sum = sum - O$H[s,j-1]*J[j-1,r];
          J[j,r] = sum/O$v[j];
        }
      }
    }
    i = i + O$n[r];
  }

  return(list(n=O$n, k=O$k, J=J, w=w));
}


#===========================================================================
# description
#   Multiplies a inverse Cholesky matrix by a vector
#   see 'Simulation of High-Dimensional t-Student Copulas'
#   by Gerard Torrent-Gironella and Josep Fortiana
# arguments
#   O: inverse Cholesky decomposition object
#   x: vector (length=sum(O$n))
# returns
#   y: inv(chol())%*%x
# example
#   sigma = matrix(nrow=3, ncol=3, data=c(0.5,0.2,0.1,0.2,0.4,0.15,0.1,0.15,0.3))
#   dims = c(3,2,1)
#   O = ccruncher.chold(sigma,dims)
#   I = ccruncher.choldinv(O)
#   x = ccruncher.multinv(I,1:6)
#   ccruncher.mult(O,x)
#===========================================================================
ccruncher.multinv <- function(O, x)
{
  k = O$k
  size = sum(O$n)

  y = vector(length=size)
  y[] = 0

  for(i in 1:size)
  {
    y[i] = O$w[i]*x[i];
  }

  i = 0
  for(r in 1:k)
  {
    sum = 0
    for(q in 1:O$n[r])
    {
      i = i+1;
      if (q > 1) 
      {
        y[i] = y[i] + sum*O$J[i,r];
      }
      sum = sum + x[i];
    }
    if (i+1 <= size)
    {
      for(j in (i+1):size)
      {
        y[j] = y[j] + sum*O$J[j,r];
      }
    }
  }

  return(y)
}

#===========================================================================
# description
#   convert a vector to a symmetric matrix
# arguments
#   v: matrix values as vector
# returns
#   symmetric matrix
# example
#   ccruncher.v2sm(1:6)
#===========================================================================
ccruncher.v2sm <- function(v)
{
  k = (-1 + sqrt(1+8*length(v)))/2
  if (abs(k-trunc(k)) > 1e-10) {
    stop("invalid vector length");
  }
  
  sm = matrix(0, nrow=k, ncol=k);
  
  s = 1
  for(i in 1:k) 
  {
    for(j in i:k)
    {
      sm[i,j] = v[s]
      sm[j,i] = sm[i,j]
      s = s+1
    }
  }
  
  return(sm)
}

#===========================================================================
# description
#   convert symmetric matrix to a vector
# arguments
#   M: symmetric matrix
# returns
#   vector
# example
#   sigma = matrix(nrow=3, ncol=3, data=c(0.5,0.2,0.1,0.2,0.4,0.15,0.1,0.15,0.3))
#   ccruncher.sm2v(sigma)
#===========================================================================
ccruncher.sm2v <- function(M)
{
  k = dim(M)[1]
  v = vector(length=k*(k+1)/2)
  s = 1
  for(i in 1:k)
  {
    for(j in i:k)
    {
      v[s] = M[i,j]
      s = s+1
    }
  }
  return(v);
}

#===========================================================================
# description
#   likelihood function
#   see 'Simulation of High-Dimensional t-Student Copulas'
#   by Gerard Torrent-Gironella and Josep Fortiana
# arguments
#   x: parameters to estimate (length=1+k*(k+1)/2 where x[1]=nu and
#      the rest of values are the correlation matrix matrix values
#   dims: elements per sector (length=k)
#   p: 1-period (eg. 1-year) default probability (length=k)
#   u: observations count data (size=Txk)
# returns
#   likelihood value
# example
#   sigma = matrix(nrow=3, ncol=3, data=c(0.5,0.2,0.1,0.2,0.4,0.15,0.1,0.15,0.3))
#   dims = c(300,200,100)
#   O = ccruncher.chold(sigma,dims)
#   nu = 15
#   p = c(0.03,0.04,0.05)
#   U = ccruncher.simcounts(O,nu,p,10000)
#   x5 = c(5,ccruncher.sm2v(sigma))
#   x15 = c(15,ccruncher.sm2v(sigma))
#   x25 = c(25,ccruncher.sm2v(sigma))
#   ccruncher.likf(x5,dims,p,U)
#   ccruncher.likf(x15,dims,p,U)
#   ccruncher.likf(x25,dims,p,U)
#===========================================================================
ccruncher.likf <- function(x,dims,p,u)
{
  ret = 0;
  #TODO: check params

  T = dim(u)[1]; # number of obervations
  k = length(dims); # number of sector
  size = sum(dims); # copula dimension
  
  # retrieve estimated parameters
  nu = x[1]
  sigma = ccruncher.v2sm(x[2:length(x)])
  sigma = ccruncher.coerce(sigma, dims)
  
  # computes required values
  alfa = qt(1-p,df=nu)
  chol = ccruncher.chold(sigma,dims);
  cinv = ccruncher.choldinv(chol);
  eigv = ccruncher.eigen(sigma, dims)
  
  for(t in 1:T)
  {
    zeta = vector(length=size)
    s = 0
    for(i in 1:k)
    {
      zeta[(s+1):(s+u[t,i])] = 100
      zeta[(s+u[t,i]+1):(s+dims[i])] = alfa[i]
      s = s + dims[i]
    }

    # combinatorial coefficients
    for(i in 1:k)
    {
      ret = ret + lgamma(dims[i]+1) - lgamma(u[t,i]+1) - lgamma(dims[i]-u[t,i]+1);
    }
    
    # ln determinant
    det = 0
    for(i in 1:length(eigv$values))
    {
      det = det + eigv$multiplicity[i]*log(eigv$values[i])
    }
    
    # gammas
    ret = ret - 0.5*det
    ret = ret + lgamma((nu+size)/2)
    ret = ret + (size-1)*lgamma(nu/2)
    ret = ret - size * lgamma((nu+1)/2)
    
    # inverse
    aux = ccruncher.multinv(cinv,zeta)
    ret = ret - (nu+size)/2 * log(1+(aux %*% aux)/nu)

    # denominator
    aux = 0
    for(i in 1:size)
    {
      aux = aux + log(1+(zeta[i]^2)/nu)
    }
    ret = ret + (nu+1)/2 * aux
  }
  
  return(ret);
}

#------------------------------------------------------------
# try to fit marginal of multivariate counts distribution
#------------------------------------------------------------
sigma = matrix(nrow=2, ncol=2, data=c(0.4,0.15,0.15,0.3))
dims = c(1000,1000)
O = ccruncher.chold(sigma,dims)
p = c(0.05,0.05)
X = ccruncher.simcounts(O,6,p,10000)

library(MASS)
fit1 = fitdistr(X[,1], "poisson")
fit2 = fitdistr(X[,1], "geometric")
fit3 = fitdistr(X[,1], "negative binomial")
fit4 = fitdistr(X[,1], "exponential")

#------------------------------------------------------------
# classic eigenvalues method vs. adapted eigenvalues method
#------------------------------------------------------------
sigma = matrix(nrow=3, ncol=3, data=c(0.4,0.99,0.1,0.99,0.5,0.3,0.1,0.3,0.6))
dims = c(2,3,1)
A <- ccruncher.cdbm(sigma, dims)

# classic eigenvalues method
EIG1 = eigen(A)
EIG1$values[EIG1$values<=0] = 0.05
A1 = EIG1$vectors %*% diag(EIG1$values) %*% t(EIG1$vectors)
A1 = ccruncher.cmo(A1)
# adapted eigenvalues method
sigma2 = ccruncher.coerce(sigma,dims)
ccruncher.cdbm(sigma2,dims)

#------------------------------------------------------------
# check spearman's rank approx
#------------------------------------------------------------
install.packages("copula")
library(copula)
t_cop <- tCopula(ccruncher.ginv(0.3,10),df=10)
x <- rcopula(t_cop,10000000)
cor(x[,1], x[,2], method = "pearson")

#------------------------------------------------------------
# compute t-student tail index
# based on 'Una metodología basada en copulas y valores 
# extremos para estimar el capital económico requerido de 
# un portafolio de créditos al menudeo' by 
# de Adán Díaz y José Carlos Ramírez
#------------------------------------------------------------
install.packages("copula")
library(copula)
nu=20
rho=0.2
tcop <- tCopula(rho,df=nu)
x <- rcopula(tcop,10000)
S_tau=cor(x,method="kendall")[2]
S_basic=sin(S_tau*pi/2)
y = x
y[,1] = sqrt(x[,1]^2+x[,2]^2)
y[,2] = asin(x[,2]/y[,1])
colnames(y) <- c("r","phi")
# exact tail value
#tail = 2*pt(-sqrt(nu+1)*sqrt(1-rho)/sqrt(1+rho), nu+1)
# approx tail value
r = 0.020
z <- y[y[,1]<r,]
sqrt(2)/(r*dim(y)[1])*sum(z[,1]*sin(2*z[,2]))

integrand <- function(x) { cos(x)^20 }
integrate(integrand, lower = (1-S_tau)*pi/4, upper = pi/2)$value / integrate(integrand, lower = 0, upper = pi/2)$value

#------------------------------------------------------------
# check some functions
#------------------------------------------------------------
M = matrix(ncol=3, nrow=3, data=c(0.5 , 0.2, 0.1, 0.2, 0.4, 0.15, 0.1, 0.15, 0.3))
dims = c(3,2,1)
O = ccruncher.chold(M, dims)
ccruncher.mult(O, 1:6)
A = ccruncher.cdbm(M, dims)
t(chol(A))%*%c(1:6)
