
#install.packages("rgl")
library(rgl)

setwd("./Pictures/")

# ================================================
# R script to create PD in the Bond example
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
# R script to create PDs from transition matrix
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

P = eigen(A12)$vectors
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

