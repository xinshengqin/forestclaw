      subroutine qinit(maxmx,maxmy,meqn,mbc,mx,my,xlower,ylower,
     &      dx,dy,q,maux,aux)
      implicit none

      integer maxmx, maxmy, meqn, mbc, mx, my, maux
      double precision xlower, ylower, dx,dy
      double precision q(1-mbc:maxmx+mbc, 1-mbc:maxmy+mbc, meqn)
      double precision aux(1-mbc:maxmx+mbc, 1-mbc:maxmy+mbc, maux)

      integer i,j
      double precision xc,yc,bot,theta, theta2, q1, R, u0
      double precision Px, Py, Pz, Rsphere, Omega, ampl,tol1, tol2
      double precision pi, xp, yp, zp

      common /comic/ Px,Py,Pz
      common /comsphere/ Rsphere, Omega
      common /cprob/ ampl, tol1, tol2

      pi = 4.d0*atan(1.d0)

c     # symmetry axis (Px,Py,Pz) set in setprob.f

       do i = 1-mbc,mx+mbc
	  xc = xlower + (i-0.5d0)*dx
          do j = 1-mbc,my+mbc
	     yc = ylower + (j-0.5d0)*dy
c
c            # set the clawpack initial values:
             bot = aux(i,j,19)
             q(i,j,1) = -bot
             q(i,j,2) = 0.d0
             q(i,j,3) = 0.d0
             q(i,j,4) = 0.d0


c            # add a small amplitude Gaussian peak:
c            # ampl is now read from setprob.data
c            ampl = 5000.d0
c            ampl = 10.d0
             call mapc2m(xc,yc,xp,yp,zp)
             theta = asin((xp*Px + yp*Py + zp*Pz) / Rsphere)
             theta2 = pi/6.d0
             q1 = exp(-1000.d0*(theta-theta2)**2)
             R = max(sqrt(xp**2 + yp**2), 1.d-10)
             u0 = 2.d2*ampl*q1 / (Rsphere*R)
             if (q1 .gt. 1.d-14) then
                q(i,j,1) = q(i,j,1) + ampl*q1
                q(i,j,2) = u0*xp*zp
                q(i,j,3) = u0*yp*zp
                q(i,j,4) = -u0*(xp**2+yp**2)
             endif
          enddo
       enddo
       return
       end