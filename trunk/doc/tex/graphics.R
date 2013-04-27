
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
plot(0:200, T[1:201,5], type='l', xlab="Time in years", 
     ylab="PD", main="Probability of Default rating BB")
grid()
dev.off();

pdf(file="bond_pd2.pdf")
plot(0:10, T[1:11,5], type='l', xlab="Time in years", 
     ylab="PD", main="Probability of Default rating BB")
axis(1, at=0:10)
grid()
dev.off();

cash = rep(1000*0.04,10)
cash[10] = cash[10] + 1000
ead = rev(cumsum(rev(cash)))
ead = c(ead,ead[10])

pdf(file="bond_ead.pdf")
plot(0:10, ead, type='s', xlab="Time in years", 
     ylab="EAD", main="Exposure At Default")
axis(1, at=0:10)
grid()
dev.off();

x = 0:100/100
t = 0:10
f <- function(x,t) {
dbeta(x,5,2)
}
z<-outer(x,t,f)
pdf(file="bond_lgd.pdf")
# see http://gallery.r-enthusiasts.com/graph/Normal_density_surface_42
persp(x, t, z, 
  theta=30, phi=20,
  r=50, d=0.1,
  expand=0.5,
  ltheta=90, lphi=180,
  #shade=0.75,
  ticktype="detailed",
  nticks=5,
  xlab="LGD",
  ylab="Time in years",
  zlab="probability",
  main="Loss Given Default")
dev.off();


