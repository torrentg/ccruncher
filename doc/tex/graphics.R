
#install.packages("rgl")
library(rgl)

setwd("./Pictures/")

# ================================================
# Bond example
# ================================================
cash = rep(1000*0.04,10)
cash[10] = cash[10] + 1000
ead = rev(cumsum(rev(cash)))
ead = c(ead,ead[10])

pdf(file="bond_ead.pdf")
plot(0:10, ead, type='s', xlab="Time in years", ylab="EAD")
axis(1, at=0:10)
grid()
dev.off();

x = 0:100/100
t = 0:10
f <- function(x,t) {
  dbeta(x,5,2)
}
z<-outer(x,t,f)

#pdf(file="bond_lgd.pdf")
# see http://gallery.r-enthusiasts.com/graph/Normal_density_surface_42
#persp(x, t, z, 
#  theta=30, phi=20,
#  r=50, d=0.1,
#  expand=0.5,
#  ltheta=90, lphi=180,
  #shade=0.75,
#  ticktype="detailed",
#  nticks=5,
#  xlab="LGD",
#  ylab="Time in years",
#  zlab="Density",
#  main="Loss Given Default")
#dev.off();

# 3d params obtained using par3d(no.readonly=TRUE)
par3d(
  FOV=30,
  ignoreExtent=FALSE,
  mouseMode=c("trackball", "zoom", "fov"),
  skipRedraw=FALSE,
  userMatrix = t(matrix(ncol=4,nrow=4,data=c(
  0.7334787,  0.6786929, 0.03721328,    1,
 -0.1770330,  0.1378910, 0.97449744,    1,
  0.6562532, -0.7213611, 0.22129118,    0,
  0.0000000,  0.0000000, 0.00000000,    1))),
  scale=c(5.9732680, 0.5973268, 2.4305289),
  zoom=1.157625,
  windowRect=c(387, 55, 943, 567),
  family="sans",
  font=1,
  cex=1,
  useFreeType=TRUE
)

plot3d(x, t, z, type='n', axes=FALSE, xlab="", ylab="", zlab="")
for(i in 1:10) {
  lines3d(x, i, dbeta(x,5,2))
}
axis3d('x--', labels=seq(0,1,0.2))
axis3d('y+-', labels=seq(0,10,1), nticks=11)
mtext3d("Time in years", 'y+-', pos=c(1.2,5,-0.5))
axis3d('z', labels=seq(0,2.5,0.5))
grid3d(side=c('z'))
title3d(
  xlab="LGD",
  ylab="",
  zlab="Density")

rgl.postscript("bond_lgd.pdf", fmt="pdf", drawText=TRUE)


# ================================================
# PDs from transition matrix example
# ================================================

ratings = c("AAA", "AA", "A", "BBB", "BB", "B", "CCC", "default")

# 1-year transition matrix
A = matrix(ncol=8, nrow=8, data=c(
  90.81, 0.7, 0.09, 0.02, 0.03, 0, 0.22, 0,
  8.33, 90.65, 2.27, 0.33, 0.14, 0.11, 0, 0,
  0.68, 7.79, 91.05, 5.95, 0.67, 0.24, 0.22, 0,
  0.06, 0.64, 5.52, 86.93, 7.73, 0.43, 1.3, 0,
  0.12, 0.06, 0.74, 5.3, 80.53, 6.48, 2.38, 0,
  0, 0.14, 0.26, 1.17, 8.84, 83.46, 11.24, 0,
  0, 0.02, 0.01, 0.12, 1.0, 4.07, 64.86, 0,
  0, 0, 0.06, 0.18, 1.06, 5.2, 19.79, 100
)/100)

P = eigen(A)$vectors
D = diag(eigen(A)$values)
# A = P %*% D %*% solve(P)

# 1-month transition matrix
A1 = P %*% (D^(1/12)) %*% solve(P)
#round(A1*100, digits=4)

qom <- function(A)
{
  n = nrow(A)
  for(i in 1:n) {
		repeat {
			s = (sum(A[i,])-1)/n
			for(j in 1:n) {
				if (A[i,j] != 0) {
					A[i,j] = A[i,j] - s
				}
			}
			if(sum(A[i,]<0)==0 & abs(sum(A[i,])-1)<1e-14) {
				break;
			} else {
				A[i,A[i,]<0]=0
			}
		}
	}
	return(A)
}

A2 = qom(A1)
#round(A2*100,digits=4)

n = nrow(A1)
A3 = A2 %*% A2
A3 = A3 %*% A3 %*% A3
A3 = A3 %*% A3
A4 = diag(n)
PD = matrix(nrow=100+1, ncol=n)
PD[1,] = 0
PD[1,n] = 1
for(t in 1:100) {
	A4 = A4 %*% A3
	PD[t+1,] = A4[,n]
}
pdf(file="pdftm1.pdf")
matplot(0:100, PD, type='l', xlab='Time in years', ylab='PD', lty=1:8, col=1:6); 
legend(80, 0.35, ratings, cex=0.9, lty=1:8, col=1:6)
grid();
dev.off();

PD = matrix(nrow=120+1, ncol=n)
PD[1,] = 0
PD[1,n] = 1
A4 = diag(n)
for(t in 1:(12*10)) {
	A4 = A4 %*% A2
	PD[t+1,] = A4[,n]
}
pdf(file="pdftm2.pdf")
matplot(0:120, PD[1:121,1:7], type='l', xlab='Time in months', ylab='PD', lty=1:8, col=1:6); 
legend(5, 0.65, ratings[1:7], cex=0.9, lty=1:8, col=1:6)
grid();
dev.off();

t = 0:100
p = A3[,8]
lambda = -log(1-p)
PD = matrix(nrow=100+1, ncol=n)
for(i in 1:n) {
	PD[,i] = 1-exp(-lambda[i]*t)
}
PD[1,8] = 1
pdf(file="pdfsv1.pdf")
matplot(0:100, PD, type='l', xlab='Time in years', ylab='PD', lty=1:8, col=1:6); 
legend(80, 0.55, ratings, cex=0.9, lty=1:8, col=1:6)
grid();
dev.off();


t = 0:120
PD = matrix(nrow=121, ncol=n)
for(i in 1:n) {
	PD[,i] = 1-exp(-lambda[i]*(t/12))
}
PD[1,8] = 1
pdf(file="pdfsv2.pdf")
matplot(t, PD[,1:7], type='l', xlab='Time in months', ylab='PD', lty=1:8, col=1:6); 
legend(5, 0.85, ratings[1:7], cex=0.9, lty=1:8, col=1:6)
grid();
dev.off();

# ================================================
# PDs from Merton's model
# ================================================

f <- function(sigma, L, t, r, mu)
{
  d2 = (log(L*exp(r*t)) + (mu-(sigma^2)/2)*t)/(sigma*sqrt(t))
  return(pnorm(-d2))
}

ratings = c("AAA", "AA", "A", "BBB", "BB", "B", "CCC", "default")
n = length(ratings)
p = A3[,n]
L = 1.2
r = 0.0
mu = r
sigma = rep(0, n)

for(i in 1:(n-1)) {
  sigma[i] = uniroot(function(sigma, L, t, r, mu, z) {f(sigma, L, t, r, mu)-z}, c(0,10), L=L, t=1, r=r, mu=mu, z=p[i])$root
}

t = 0:100
PD = matrix(nrow=100+1, ncol=n, 0)
PD[,n] = 1
for(i in 1:(n-1)) {
  PD[1,i] = 0
  for(j in 1:100) {
    PD[j+1,i] = f(sigma[i], L, j, r, mu)
  }
}

pdf(file="pdfmm1.pdf")
par(mar=c(5,4,1,1))
matplot(t, PD, type='l', xlab='Time in years', ylab='PD', lty=1:8, col=1:6); 
legend(80, 0.35, ratings, cex=0.9, lty=1:8, col=1:6)
grid();
dev.off();

t = 0:120
PD = matrix(nrow=121, ncol=n, 0)
PD[,n] = 1
for(i in 1:(n-1)) {
  PD[1,i] = 0
  for(j in 1:120) {
    PD[j+1,i] = f(sigma[i], L, j/12, r, mu)
  }
}

pdf(file="pdfmm2.pdf")
par(mar=c(5,4,1,1))
matplot(t, PD[,1:7], type='l', xlab='Time in months', ylab='PD', lty=1:8, col=1:6); 
legend(5, 0.5, ratings[1:7], cex=0.9, lty=1:8, col=1:6)
grid();
dev.off();


# ================================================
# Correlation Coefficient--Bivariate Normal Distribution
# see http://mathworld.wolfram.com/CorrelationCoefficientBivariateNormalDistribution.html
# ================================================
# f <- function(beta, r, rho, n){1/(cosh(beta)-rho*r)^(n-1)}

# ccbn <- function(r, rho, n)
# {
  # x = integrate(f, lower=0, upper=Inf, r, rho, n)$value
  # return(x/pi * (n-2) * (1-r^2)^((n-4)/2) * (1-rho^2)^((n-1)/2))
# }
# vccbn <- Vectorize(ccbn, "r")

# x = seq(-1,+1,0.01)
# pdf(file="ccbnd.pdf")
# plot(x,vccbn(x,0.2,20), type='l', xlab="correlation", ylab="density")
# grid()
# dev.off();

# ================================================
# Bayessian inference using Metropolis-Hastings example
# ================================================
lnlik <- function(y, a, b)
{
	ret = length(y) * (lgamma(a+b) - lgamma(a) - lgamma(b))
	ret = ret + (a-1)*sum(log(y))
	ret = ret + (b-1)*sum(log(1-y))
	return(ret)
}

bimh <- function(n, y, pa, pb)
{
	values = matrix(nrow=n, ncol=2, 0)
	values[1,] = c(1, 1)
	sigmas = matrix(nrow=n, ncol=2, 0)
	sigmas[1,] = c(0.1, 0.1)
	
	for(i in 2:n)
	{
		x = values[i-1,]
		
		# param alpha
		xa = rnorm(1, mean=x[1], sd=sigmas[i-1,1])
		lnar = + lnlik(y, xa, x[2]) + dunif(xa, min=0, max=100, log=TRUE) -
           lnlik(y, x[1], x[2]) - dunif(x[1], min=0, max=100, log=TRUE)
		lnu = log(runif(1))
		if (lnu < lnar) {
			x[1] = xa
			sigmas[i,1] = abs(sigmas[i-1,1] * (1 + 1/(pa*i)))
		} else {
			sigmas[i,1] = abs(sigmas[i-1,1] * (1 - 1/((1-pa)*i)))
		}
		
		# param beta
		xb = rnorm(1, mean=x[2], sd=sigmas[i-1,2])
		lnar = + lnlik(y, x[1], xb) + dunif(xb, min=0, max=30, log=TRUE) -
           lnlik(y, x[1], x[2]) - dunif(x[2], min=0, max=30, log=TRUE)
		lnu = log(runif(1))
		if (lnu < lnar) {
			x[2] = xb
			sigmas[i,2] = abs(sigmas[i-1,2] * (1 + 1/(pb*i)))
		} else {
			sigmas[i,2] = abs(sigmas[i-1,2] * (1 - 1/((1-pb)*i)))
		}
		
		# simulated value
		values[i,] = x
	}
	return(list(values=values,sigmas=sigmas))
}

y = rbeta(100, 2, 5)
x = bimh(25000, y, 0.3, 0.3)
#apply(x$values[5000:nrow(x$values),],2,mean)
pdf(file="mhbd1.pdf", width=7, height=2)
#par(oma=c(0,0,0,0))
par(mar=c(2,2,1,1))
matplot(x$values[1:1000,], type='l', xlab="iteration", ylab=""); grid()
dev.off()

pdf(file="mhbd2.pdf", width=7, height=2)
par(mar=c(2,2,1,1))
matplot(x$sigmas[1:1000,], type='l', xlab="iteration", ylab=""); grid()
dev.off()

pdf(file="mhbd3.pdf", width=7, height=3.5)
par(mar=c(2,2,1,1))
hist(x$values[5000:nrow(x$values),1],breaks=20,xlab="parameter" ~alpha, main="");
dev.off()

pdf(file="mhbd4.pdf", width=7, height=3.5)
par(mar=c(2,2,1,1))
hist(x$values[5000:nrow(x$values),2],breaks=20,xlab="parameter" ~beta, main="");
dev.off()

# ================================================
# Bayessian inference example (1000 obs)
# ================================================
skip=2000

fcalib <- function(name)
{
	X <- read.csv(name %&% ".out", header=TRUE, sep="\t", comment.char="#")
	n=nrow(X)

	pdf(file=name %&% "1.pdf", width=2.3, height=1.8)
	par(mar=c(2,2,0.5,0))
	hist(X[(skip+1):n,1], breaks=20, freq=TRUE, xlab="Parameter "~nu, ylab="Frequency", main=""); 
	dev.off()

	pdf(file=name %&% "2.pdf", width=7, height=2)
	par(mfrow=c(1,3));
	par(mar=c(4,2,0.5,0))
	hist(X[(skip+1):n,2], breaks=20, freq=TRUE, xlab="Parameter "~W[1], ylab="Frequency", main=""); 
	hist(X[(skip+1):n,3], breaks=20, freq=TRUE, xlab="Parameter "~W[2], ylab="Frequency", main=""); 
	hist(X[(skip+1):n,4], breaks=20, freq=TRUE, xlab="Parameter "~W[3], ylab="Frequency", main=""); 
	dev.off()

	pdf(file=name %&% "3.pdf", width=7, height=2)
	par(mfrow=c(1,3));
	par(mar=c(4,2,1,0))
	hist(X[(skip+1):n,5], breaks=20, freq=TRUE, xlab="Parameter "~R[1][2], ylab="Frequency", main=""); 
	hist(X[(skip+1):n,6], breaks=20, freq=TRUE, xlab="Parameter "~R[1][3], ylab="Frequency", main=""); 
	hist(X[(skip+1):n,7], breaks=20, freq=TRUE, xlab="Parameter "~R[2][3], ylab="Frequency", main=""); 
	dev.off()
}

fcalib("calib1")
fcalib("calib3")

# ================================================
# CCruncher example (test04)
# ================================================
# probability density function (pdf)
df <- function(x, p, w) {
	ifelse(x<=0|x>=1, 0, sqrt(1-w^2)/w * exp(qnorm(x)^2/2 - (qnorm(p)-sqrt(1-w^2)*qnorm(x))^2/(2*w^2)))
}

# reading simulated data
portfolio = read.csv("data/test04-1000/portfolio.csv", comment.char="#")

numsims = nrow(portfolio)
numobligors = 1000
numbreaks = min(100,length(tabulate(portfolio[,1]+1)))
h = hist(portfolio[,1], breaks=numbreaks)
x = h$mids/numobligors
y1 = h$counts/numsims
y2 = df(x,0.1,0.2)
y2 = y2 / sum(y2)

# plotting data
pdf(file="test04-1.pdf", width=7, height=4)
par(mar=c(4,4,1,1))
plot(x, y2, type='l', col=2, xlab="portfolio loss (%)", ylab="density", main="")
grid()
lines(x, y1, type='l', col=1)
legend(0.25, 0.05, c("CCruncher", "LHP"), lty=c(1,1), col= 1:2, bg="white", cex=0.7)
dev.off()

# reading simulated data
portfolio = read.csv("data/test04-100/portfolio.csv", comment.char="#")

numsims = nrow(portfolio)
numobligors = 100
numbreaks = min(100,length(tabulate(portfolio[,1]+1)))
h = hist(portfolio[,1], breaks=numbreaks)
x = h$mids/numobligors
y1 = h$counts/numsims
y2 = df(x,0.1,0.2)
y2 = y2 / sum(y2)

# plotting data
pdf(file="test04-2.pdf", width=7, height=4)
par(mar=c(4,4,1,1))
plot(x, y2, type='l', col=2, xlab="portfolio loss (%)", ylab="density", main="")
grid()
lines(x, y1, type='l', col=1)
legend(0.25, 0.1, c("CCruncher", "LHP"), lty=c(1,1), col= 1:2, bg="white", cex=0.7)
dev.off()

# ================================================
# CCruncher example (test05)
# ================================================
portfolio = read.csv("data/test05/portfolio.csv", comment.char="#")
d = density(portfolio[,1])
pdf(file="test05.pdf", width=7, height=3)
par(mar=c(4,4,1,1))
plot(d, main="", xlab="Portfolio Loss"); 
grid()
dev.off()


# ================================================
# LHS example
# ================================================

 rlhs <- function(nblocks, icdf, ...) 
 {
   ret = rep(NA,nblocks)
   for(i in 1:nblocks) {
     u = runif(1, min=(i-1)/nblocks, max=i/nblocks)
     ret[i] = icdf(u, ...)
   }
   ret = sample(ret)
   return(ret)
 }

 x1 = rnorm(100, mean=0, sd=1)
 cdf1 = ecdf(x1)
 pdf(file="lhs1.pdf", width=7, height=3)
 par(mar=c(2,2,1,1))
 plot(cdf1, verticals=TRUE, pch=-1, ylab="prob", main="");
 grid()
 dev.off()

 x2 = rlhs(100, qnorm, mean=0, sd=1)
 cdf2 = ecdf(x2)
 pdf(file="lhs2.pdf", width=7, height=3)
 par(mar=c(2,2,1,1))
 plot(cdf2, verticals=TRUE, pch=-1, ylab="prob", main="");
 grid()
 dev.off()

# ================================================
# paramu example
# ================================================
X <- read.csv("paramu.out", header=TRUE, sep="\t", comment.char="#")
n=nrow(X)
skip = 5000
pdf(file="paramu-1.pdf", width=7, height=2)
par(mfrow=c(1,4));
par(mar=c(4,2,0.5,0))
hist(X[(skip+1):n,1], breaks=20, freq=TRUE, xlab="Parameter "~nu, ylab="Frequency", main=""); 
hist(X[(skip+1):n,2], breaks=20, freq=TRUE, xlab="Parameter "~W[1], ylab="Frequency", main=""); 
hist(X[(skip+1):n,3], breaks=20, freq=TRUE, xlab="Parameter "~W[2], ylab="Frequency", main=""); 
hist(X[(skip+1):n,4], breaks=20, freq=TRUE, xlab="Parameter "~R[1][2], ylab="Frequency", main=""); 
dev.off()

pdf(file="paramu-2.pdf", width=7, height=2)
par(mfrow=c(1,4));
par(mar=c(4,2,0.5,0))
acf(X[(skip+1):n,1], lag.max=3000, xlab="Parameter "~nu, main=""); 
acf(X[(skip+1):n,2], lag.max=3000, xlab="Parameter "~W[1], main=""); 
acf(X[(skip+1):n,3], lag.max=3000, xlab="Parameter "~W[2], main=""); 
acf(X[(skip+1):n,4], lag.max=3000, xlab="Parameter "~R[1][2], main=""); 
dev.off()

# prune values
Y = X[skip:n,1:4]
Y = Y[(1:nrow(Y))%%3000==0,]
fprint <- function(i, params) {
	str = "mkdir data/MH" %&% sprintf("%03d",i)
	str = str %&% "; build/ccruncher-cmd -o data/MH" %&% sprintf("%03d",i)
	str = str %&% " -D NU=" %&% params[1]
	str = str %&% " -D W1=" %&% params[2]
	str = str %&% " -D W2=" %&% params[3]
	str = str %&% " -D R12=" %&% params[4]
	str = str %&% " samples/test05.xml;"
	return(str)
}
for(i in 1:nrow(Y)) {
	cat(fprint(i, Y[i,]), "\n")
}

# plot density
 getRisk <- function(dir)
 {
   filename = paste(dir, "/portfolio.csv", sep="")
   data <- read.csv(filename, comment.char="#")
   X = sort(data[,1])
   n = length(X)
   Y = X[as.integer(n*0.99):n]
   ES99 = mean(Y)
   sde = sqrt(var(Y)/length(Y))
   return(c(ES99,sde))
 }

 dirs = dir("data", pattern="MH[[:digit:]{3}]*", full.names=TRUE)
 values = matrix(ncol=2,nrow=length(dirs))
 colnames(values) = c("ES99", "stderr")
 for(i in 1:length(dirs)) {
   values[i,] = getRisk(dirs[i])
 }
 
 pdf(file="paramu-5.pdf", width=7, height=3)
 par(mar=c(2,4,0.5,0))
 plot(density(values[,1]), main="", ylab="Density")
 grid()
 dev.off()


# ================================================
# implied covariance estimator
# ================================================

 pri <- function(i, K, N)
 {
   if (sum(dim(K) != dim(N))) stop("K and N dim differs")
   if (i <= 0 | ncol(K) < i) stop("i out-of-range")
   ret = sum(K[,i])/sum(N[,i])
   return(ret)
 }

 prij <- function(i, j, K, N)
 {
   if (sum(dim(K) != dim(N))) stop("K and N dim differs")
   if (i <= 0 | ncol(K) < i) stop("i out-of-range")
   if (j <= 0 | ncol(K) < j) stop("j out-of-range")
   if (i != j) {
     ret = sum(K[,i]*K[,j])/sum(N[,i]*N[,j])
   }
   else {
     ret = sum(K[,i]*(K[,i]-1))/sum(N[,i]*(N[,i]-1))
   }
   return(ret)
 }

 f <- function(r, nu, p1, p2, p12)
 {
   q1 = qt(p1, df=nu)
   q2 = qt(p2, df=nu)
   R = matrix(nrow=2, ncol=2, 1)
   R[1,2] = R[2,1] = r
   val = pmvt(lower=c(-Inf,-Inf), upper=c(q1,q2), corr=R, df=nu)
   return(p12-val)
 }

 implied_covariance <- function(nu, K, N)
 {
   if (sum(dim(K) != dim(N))) stop("K and N dim differs")
   n = ncol(K)
   ret = matrix(nrow=n, ncol=n, 0)
   for(i in 1:n) {
     for(j in i:n) {
       p1 = pri(i, K, N)
       p2 = pri(j, K, N)
       p12 = prij(i, j, K, N)
       ret[i,j] = uniroot(f, nu, p1, p2, p12, interval=c(-0.9,0.9))$root
       ret[j,i] = ret[i,j]
     }
   }
   return(ret)
 }

exact <- function(rcount)
{
  k = length(rcount$w)
  ret = matrix(ncol=k, nrow=k, 0)
  for(i in 1:k) {
    for(j in i:k) {
      ret[i,j] = ret[j,i] = rcount$w[i]*rcount$w[j]*rcount$R[i,j]
    }
  }
  return(ret)
}

#----------
source('/home/gerard/projects/ccbinf/bin/ccbinf.R')

# 2-FACTORS, 1-RATING, GAUSSIAN
p = c(0.05)
w = c(0.3, 0.25)
D = matrix(ncol=1, nrow=2, data=c(750, 750))
R = matrix(ncol=2, nrow=2, data=c(1.0,0.1,0.1,1.0))
nu = 10000
rcount = ccruncher.rcount(1000, p, D, w, R, nu)

# 2-FACTORS, 2-RATINGS, NON-GAUSSIAN
p = c(0.025, 0.07)
w = c(0.3, 0.25)
D = diag(c(1250,750))
R = matrix(ncol=2, nrow=2, data=c(1.0,0.1,0.1,1.0))
nu = 15
rcount = ccruncher.rcount(1000, p, D, w, R, nu)

# 4-FACTORS, 4-RATINGS, NON-GAUSSIAN
p = c(0.05, 0.025, 0.03, 0.045)
w = c(0.3, 0.25, 0.4, 0.8)
D = diag(c(1000,2000,3000,4000))
R = matrix(ncol=4, nrow=4, data=c(1.0,0.1,0.2,0.4, 0.1,1.0,0.4,0.3, 0.2,0.4,1.0,0.2, 0.4,0.3,0.2,1.0))
nu = 10
rcount = ccruncher.rcount(1000, p, D, w, R, nu)

# changing format
m = length(rcount$p)
k = length(rcount$w)
n = nrow(rcount$K)
K = matrix(nrow=n, ncol=k)
N = matrix(nrow=n, ncol=k, byrow=TRUE, rowSums(rcount$D))
for(i in 1:n) {
  for(j in 1:k) {
    K[i,j] = sum(rcount$K[i,((j-1)*m+1):(j*m)])
  }
}

# computing estimators
exact(rcount)
implied_covariance(rcount$nu, K, N)

# ================================================
# risk disaggregation
# ================================================
names = c("S1-A", "S1-B", "S1-C", "S2-A", "S2-B", "S2-C")
el = c(8.34436, 16.5874, 33.3918, 8.35995, 16.7023, 33.1902)
es99 = c(48.1333, 66.6505, 90.7877, 40.5039, 58.075, 80.2446)

labels1 = names
for(i in 1:length(el)) {
  labels1[i] = paste(names[i], "\n", round(100*el[i]/sum(el),2), "%")
}

labels2 = names
for(i in 1:length(el)) {
  labels2[i] = paste(names[i], "\n", round(100*es99[i]/sum(es99),2), "%")
}

pdf(file="disaggregation1.pdf", width=7, height=7)
par(mar=c(0,0,0,0))
pie(el, labels1, radius=0.8)
dev.off()

pdf(file="disaggregation2.pdf", width=7, height=7)
par(mar=c(0,0,0,0))
pie(es99, labels2, radius=0.8)
dev.off()

# ================================================
# sensitivity analysis
# ================================================

#!/bin/bash
if [ $# -ne 1 ]; then
    echo "error: invalid number of arguments.";
    exit 1;
fi
function ctrl_c() {
	echo "** Trapped CTRL-C"
	exit 1;
}
trap ctrl_c INT
export LC_NUMERIC=C
for NUM in $(seq -f "%03.2f" 0 0.05 1;)
do
	echo "simulating $1=${NUM} ...";
	mkdir data/SA/$1.${NUM};
	build/ccruncher-cmd -wo data/SA/$1.${NUM} \
		-D lhs=false -D seed=10 -D $1=${NUM} \
		samples/test05.xml > data/SA/$1.${NUM}/ccruncher.out;
	if [ $? -ne 0 ]; then
        rm -rvf data/SA/$1.${NUM};
    fi
done

#/usr/bin/R

 getRisk <- function(path)
 {
   filename = paste(path, "/portfolio.csv", sep="")
   data <- read.csv(filename, comment.char="#")
   X = sort(data[,1])
   n = length(X)
   Y = X[as.integer(n*0.99):n]
   ES99 = mean(Y)
   sde = sqrt(var(Y)/length(Y))
   return(c(ES99,sde))
 }

 getValues <- function(path, prefix)
 {
   len = nchar(path)+nchar(prefix)+3
   pat = paste(prefix,"[.](.*)",sep="")
   dirs = dir(path, pattern=pat , full.names=TRUE)
   values = matrix(ncol=3,nrow=length(dirs))
   colnames(values) = c("NU", "ES99", "stderr")
   for(i in 1:length(dirs)) {
     values[i,1] = as.double(substr(dirs[i], len, 1000))
     values[i,2:3] = getRisk(dirs[i])
   }
   values = values[order(values[,1]),]
   return(values)
 }

 plotValues <- function(values, k=1)
 {
   fit <- smooth.spline(values[,1], values[,2])
   dfit = predict(fit, values[,1], deriv=1)$y
   nf = 1/sqrt(dfit^2 + 1)
   x.high = fit$x - dfit * nf * 1.96 * values[,3] * k
   y.high = fit$y +    1 * nf * 1.96 * values[,3] * k
   x.low  = fit$x + dfit * nf * 1.96 * values[,3] * k
   y.low  = fit$y -    1 * nf * 1.96 * values[,3] * k
   plot(values[,1:2]) #, ylim=c(250,700)
   polygon(c(x.high, rev(x.low)), c(y.high, rev(y.low)),
       col = "grey80", border = NA)
   lines(fit$x, fit$y, col="red")
   points(values[,1:2])
   grid()
 }

 VNU = getValues("data/SA", "NU")
 VW1 = getValues("data/SA", "W1")
 VW2 = getValues("data/SA", "W2")
 VR12 = getValues("data/SA", "R12")
 
 #par(mfrow=c(2,2))
 
 pdf(file="sensi-w1.pdf", width=7, height=7)
 par(mar=c(2,2,0.5,0.5))
 plotValues(VW1[1:21,], k=0.01)
 points(VW1[9,1], VW1[9,2], pch=21, bg="black")
 dev.off()
 
 pdf(file="sensi-w2.pdf", width=7, height=7)
 par(mar=c(2,2,0.5,0.5))
 plotValues(VW2[1:21,], k=0.01)
 points(VW2[8,1], VW2[8,2], pch=21, bg="black")
 dev.off()
 
 pdf(file="sensi-r12.pdf", width=7, height=7)
 par(mar=c(2,2,0.5,0.5))
 plotValues(VR12, k=0.05)
 points(VR12[25,1], VR12[25,2], pch=21, bg="black")
 dev.off()
 
 pdf(file="sensi-nu.pdf", width=7, height=7)
 par(mar=c(2,2,0.5,0.5))
 plotValues(VNU)
 points(VNU[11,1], VNU[11,2], pch=21, bg="black")
 dev.off()
 
 VSIZE1 = getValues("data/SA", "SIZE1")
 x = VSIZE1[,1]
 y = VSIZE1[,2]/VSIZE1[,1]

 pdf(file="sensi-size.pdf", width=7, height=3)
 par(mar=c(4,4,0.5,0.1))
 plot(x, y, yaxt="n", xlab="Number of obligors", ylab=~ES[99])
 axis(2, at=pretty(y), paste0(pretty(y)*100, " %"), las=TRUE)
 nlmod <- nls(y ~ B+A/(x))
 lines(x, predict(nlmod), col = 2)
 points(x[20], y[20], pch=21, bg="black")
 grid()
 dev.off()

 #--------

 VSIZE2 = getValues("data/SA", "SIZE2")
 x = VSIZE2[,1]
 y = VSIZE2[,2]/1000
 data <- read.csv("data/SA/SIZE2.500/sector-rating.csv", comment.char="#")
 L = cbind(rowSums(data[,1:3]), rowSums(data[,4:6]))
 W = matrix(nrow=nrow(VSIZE2), ncol=4)
 W[,1] = VSIZE2[,1]
 W[,2] = 1000-W[,1]
 for(i in 1:nrow(W)) {
    # see below for eles() function definition
    W[i,3:4] = eles(L, W[i,1:2]/1000, c(500,500))
 }

 pdf(file="sensi-pct.pdf", width=7, height=3)
 par(mar=c(4,4,0.5,0.1))
 plot(x, y, yaxt="n", xaxt="n", xlab="Number of obligors", ylab=~ES[99], type='o')
 axis(2, at=pretty(y), paste0(pretty(y)*100, " %"), las=TRUE)
 axis(1, at=pretty(x,10), paste0("\n",pretty(x,10),"+\n",1000-pretty(x,10)," "), mgp=c(0,1.5,0), las=TRUE)
 #nlmod <- nls(y ~ a+b*x+c*x^2+d*x^3+e*x^4, start = list(a=1,b=1,c=1,d=0,e=0))
 #lines(x, predict(nlmod), col = 2)
 points(x[10], y[10], pch=21, bg="black")
 points(W[,1],W[,4]/500, pch=3, type='o', col="red")
 legend(200, 0.45, c("Simulated", "Approximated"), lty=c(1,1), pch=c(21,3), col=c("black","red"), bg="white", cex=0.7)
 grid()
 dev.off()
 
 par(mar=c(4,4,0.5,0.1))
 plot(x, W[,4]/500-y,  yaxt="n", xaxt="n", type='o', xlab="Number of obligors", ylab=~ES[99]~" error")
 axis(2, at=pretty(W[,4]/500-y), paste0(pretty(W[,4]/500-y)*100, " %"), las=TRUE)
 axis(1, at=pretty(x,10), paste0("\n",pretty(x,10),"+\n",1000-pretty(x,10)," "), mgp=c(0,1.5,0), las=TRUE)
 grid()
 
# ================================================
# portfolio optimization
# ================================================

f <- function()
{
  ret = NULL
  p = 0:10
  a = rep(0, 6)
    
  for(i1 in 1:11) {
    a[1] = i1
    for(i2 in 1:11) {
      if (sum(p[a]) + p[i2] > 10.001) next
      a[2] = i2
      for(i3 in 1:11) {
        if (sum(p[a]) + p[i3] > 10.001) next
        a[3] = i3
        for(i4 in 1:11) {
          if (sum(p[a]) + p[i4] > 10.001) next
          a[4] = i4
          for(i5 in 1:11) {
            if (sum(p[a]) + p[i5] > 10.001) next
            a[5] = i5
            for(i6 in 1:11) {
              if (sum(p[a]) + p[i6] != 10) next
              a[6] = i6
              ret = rbind(ret, p[a]/10)
              a[6] = 0
            }
            a[5] = 0
          }
          a[4] = 0
        }
        a[3] = 0
      }
      a[2] = 0
    }
    a[1] = 0
  }
  
  return(ret)
}

# return EL and ES99 as percentage
eles <- function(losses, weights, exposures, level=0.99)
{
  if (ncol(losses) != length(weights)) stop("data frame cols and weight length differs")
  if (level <= 0 || level >= 1) stop("ES level out-of-range (0,1)")
  n = nrow(losses)
  k = ncol(losses)
  loss = rep(0, n)
  for(i in 1:k) {
    loss = loss + losses[,i]*weights[i]
  }
  loss = sort(loss)
  el = mean(loss)
  es = mean(loss[as.integer(n*level):n])
  return(c(el,es))
}


E = c(167, 166, 167, 167, 167, 166)
L <- read.csv("data/sector-rating.csv", comment.char="#")
W = f()
W = cbind(W, rep(0,nrow(W)), rep(0,nrow(W)))
colnames(W) = c(colnames(L), "el", "es")
for(i in 1:nrow(W)) {
    W[i,7:8] = eles(L, W[i,1:6], E)/166.66
}

p = NULL
p = rbind(p, c(383.82/1000, 116.58/1000))
p = rbind(p, c(383.82/1000,  64.50/1000))
p = rbind(p, c(374.00/1000, 116.58/1000))
p = rbind(p, rev(W[2569,7:8]))
p = rbind(p, rev(W[3003,7:8]))
p = rbind(p, rev(W[1001,7:8]))
p = rbind(p, rev(W[ 286,7:8]))
p = rbind(p, rev(W[ 203,7:8]))
p = rbind(p, rev(W[1151,7:8]))
p = rbind(p, rev(W[1940,7:8]))
plabs = c("", "", "", "A", "B", "C", "D", "E", "F", "G")


pdf(file="optim1.pdf", width=7, height=7)
par(mar=c(4,4,0.5,0.5))
plot(W[,8], W[,7], xlab=~ES[99], ylab="EL", pch='.', yaxt="n", xaxt="n", xlim=c(0.25,0.65))
axis(1, at=pretty(W[,8]), paste0(pretty(W[,8])*100, " %"), las=TRUE)
axis(2, at=pretty(W[,7]), paste0(pretty(W[,7])*100, " %"), las=TRUE)
points(p[4:nrow(p),], pch=21)
points(p[1,1],p[1,2], pch=21, bg="black")
arrows(p[1,1],p[1,2],p[2,1],p[2,2], lty="solid", length=0.1)
arrows(p[1,1],p[1,2],p[3,1],p[3,2], lty="solid", length=0.1)
hpts <- chull(W[,8], W[,7])
hpts <- c(hpts, hpts[1])
lines(W[hpts,8], W[hpts,7])
text(p[,1]+0.01*c(0,0,0,-1,1,1,1,-1,-1,-1), p[,2], plabs)
grid()
#identify(W[,8], W[,7], 1:nrow(W))
dev.off()


# ================================================
# market data (illustration default)
# ================================================
library(mvtnorm)

set.seed(6)
sigma = c(0.15, 0.13, 0.17)
R = matrix(nrow=3, ncol=3, byrow=TRUE, data=c(0.4,0.3,0.25,0.3,0.35,0.15,0.25,0.15,0.3))
levels = c(0.75, 0.75, 0.75)

tsteps = 250
nassets = length(sigma)

mu = 0
w = sqrt(diag(R))
diag(R) = 1
drift = mu - (sigma^2)/2
S = matrix(0, nrow=tsteps+1, ncol=nassets)
S[1,] = rep(1,nassets)
for(t in 1:tsteps)
{
  Z = rmvnorm(1, rep(0,nassets), R)
  returns = drift/tsteps + sigma * sqrt(1/tsteps) * Z
  S[t+1,] = S[t,]*exp(returns)
}

pdf(file="market1.pdf", width=7, height=3)
par(mar=c(4,4,0.5,0.1), cex.axis=0.75, cex.lab=0.75, cex.main=1, cex.sub=1)
plot(S[,1],type='l',col=2, xlab="Time (in days)", ylab="Asset price (in %)", yaxt="n", ylim=c(min(S),max(S)))
axis(2, at=pretty(c(S[,1],S[,2],S[,3])), paste0(pretty(c(S[,1],S[,2],S[,3]))*100, " %"), las=TRUE)
lines(S[,2],type='l',col=3)
lines(S[,3],type='l',col=4)
lines(rep(1.0,251), lty=2, col=24)
lines(rep(0.9,251), lty=2, col=2)
points(251,S[251,1], pch=19, col=2)
points(251,S[251,2], pch=19, col=3)
points(251,S[251,3], pch=19, col=4)
grid()

dev.off()

# ================================================
# distribution defaults using market model
# each distribution corresponds to a threshold
# ================================================

# read simulated defaults in a file (1 column)
readDefaults <- function(filename)
{
  data <- read.csv(filename, header=FALSE, comment.char="#")
  return(data[,1])
}

pdf(file="market2.pdf", width=7, height=4)
par(mar=c(4,4,0.5,0.1), cex.axis=0.75, cex.lab=0.75, cex.main=1, cex.sub=1)

leglabs = NULL
myxbreaks = 0.01*(0:100)
myybreaks = 0.001*(0:150)
filenames = list.files(pattern = "*.out")
plot(0, 0, xlim=c(0,1), ylim=c(0,0.15), yaxt="n", xaxt="n", xlab='% of defaults', ylab='Density')
axis(1, at=pretty(myxbreaks), paste0(pretty(myxbreaks)*100, " %"), las=TRUE)
axis(2, at=pretty(myybreaks), paste0(pretty(myybreaks)*100, " %"), las=TRUE)
numcol = 1
for(i in 1:length(filenames)) {
  if ((i%%2) != 1) next
  filename = filenames[i]
  threshold = as.double(substr(filename,11,14))
  if (threshold < 0.67) next
  D = readDefaults(filename)
  D = D/2000
  h = hist(D, breaks=myxbreaks, plot=FALSE)
  x = h$mids
  y = h$counts/length(D)
  #lines(x, y, type='l', col=i)
  #lines(spline(x, y, xmin=0, xmax=1, method='n', n=100), type='l', col=i )
  # smoothing histograms
  lo <- loess(y~x, span=0.20, data.frame(x=x, y=y))
  lines(x, sapply(predict(lo, data.frame(x=x)), max, 0), type='l', col=numcol )
  leglabs = c(leglabs, paste(threshold*100,"%",sep=""))
  numcol = numcol+1
}
legend(0.3, 0.15, leglabs, cex=0.7, lty=1, col=1:numcol, ncol=3)
grid()

dev.off()

# ================================================
# probability of default depending of threshold
# ================================================

# read simulated defaults in a file (1 column)
readDefaults <- function(filename)
{
  data <- read.csv(filename, header=FALSE, comment.char="#")
  return(data[,1])
}

pdf(file="market3.pdf", width=7, height=4)
par(mar=c(4,4,0.5,0.1), cex.axis=0.75, cex.lab=0.75, cex.main=1, cex.sub=1)

myxbreaks = 0.01*(40:100)
myybreaks = 0.01*(0:100)
filenames = list.files(pattern = "*.out")
x = NULL
y = NULL
for(i in 1:length(filenames)) {
  filename = filenames[i]
  threshold = as.double(substr(filename,11,14))
  #if (threshold < 0.67) next
  D = readDefaults(filename)
  x = c(x, threshold)
  y = c(y, mean(D/2000))
}
plot(x, y, xlim=c(0.4,1), ylim=c(0,1), yaxt="n", xaxt="n", xlab='Threshold', ylab='Probability of default', type='l')
axis(1, at=pretty(myxbreaks), paste0(pretty(myxbreaks)*100, " %"), las=TRUE)
axis(2, at=pretty(myybreaks), paste0(pretty(myybreaks)*100, " %"), las=TRUE)
grid()

dev.off()


# ================================================
# calibration independent on threshold
# ================================================

# read simulated values in a file (4 columns)
readValues <- function(filename)
{
  data <- read.csv(filename, header=TRUE, comment.char="#", sep="\t")
  return(data[,2])
}

pdf(file="market4.pdf", width=7, height=4)
par(mar=c(4,4,0.5,0.1), cex.axis=0.75, cex.lab=0.75, cex.main=1, cex.sub=1)

myxbreaks = 0.01*(40:100)
myybreaks = 0.01*(20:30)
filenames = list.files(pattern = "*[0-9]\\.out")
x = NULL
y = NULL
z = NULL
for(i in 1:length(filenames)) {
  filename = filenames[i]
  threshold = as.double(substr(filename,11,14))
  #if (threshold < 0.67) next
  W = readValues(filename)
  x = c(x, threshold)
  y = c(y, mean(W[10000:50000]))
  z = c(z, var(W[10000:50000]))
}
plot(x, y, xlim=c(0.4,1), ylim=c(0.2,0.3), yaxt="n", xaxt="n", xlab='Threshold', ylab='W', type='l')
axis(1, at=pretty(myxbreaks), paste0(pretty(myxbreaks)*100, " %"), las=TRUE)
axis(2, at=pretty(myybreaks), paste0(pretty(myybreaks)*100, " %"), las=TRUE)
grid()

dev.off()

