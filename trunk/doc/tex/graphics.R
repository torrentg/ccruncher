
#install.packages("rgl")
library(rgl)

setwd("./Pictures/")

# ================================================
# PD in the Bond example
# ================================================
ratings = c("AAA", "AA", "A", "BBB", "BB", "B", "CCC", "default")

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

n=201
B = diag(8)
T = matrix(nrow=n, ncol=8, 0)
colnames(T) <- ratings

for(i in 2:n)
{
  B = B %*% A
  T[i,] = B[,8]
}

pdf(file="bond_pd1.pdf")
plot(0:200, T[1:201,5], type='l', xlab="Time in years", ylab="PD")
grid()
dev.off();

pdf(file="bond_pd2.pdf")
plot(0:10, T[1:11,5], type='l', xlab="Time in years", ylab="PD")
axis(1, at=0:10)
grid()
dev.off();

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
# Correlation Coefficient--Bivariate Normal Distribution
# see http://mathworld.wolfram.com/CorrelationCoefficientBivariateNormalDistribution.html
# ================================================
f <- function(beta, r, rho, n){1/(cosh(beta)-rho*r)^(n-1)}

ccbn <- function(r, rho, n)
{
  x = integrate(f, lower=0, upper=Inf, r, rho, n)$value
  return(x/pi * (n-2) * (1-r^2)^((n-4)/2) * (1-rho^2)^((n-1)/2))
}
vccbn <- Vectorize(ccbn, "r")

x = seq(-1,+1,0.01)
pdf(file="ccbnd.pdf")
plot(x,vccbn(x,0.2,20), type='l', xlab="correlation", ylab="density")
grid()
dev.off();

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
source('/home/gerard/projects/ccbinf/bin/ccbinf.R')
skip=2000

fcalib <- function(name)
{
	X <- import(name %&% ".out"); n=nrow(X)

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

