      subroutine amg_compute_residual(mx,my,mbc,        
     &     xlower,ylower,dx,dy,u,r)
      implicit none

      integer mx,my,mbc
      double precision r(mx,my)
      double precision u(1-mbc:mx+mbc,1-mbc:my+mbc)
      double precision xlower,ylower,dx,dy

      double precision x,y, pi, f, fx(mx),fy(my)
      integer i,j
      double precision dux2, duy2, lap

      pi = 4.0*atan(1.d0)

      do i = 1,mx 
        x = xlower + (i-0.5)*dx
        fx(i) = sin(2*pi*x)
      end do

      do j = 1,my 
        y = ylower + (j-0.5)*dy
        fy(j) = sin(2*pi*y)
      end do

      do i = 1,mx
        do j = 1,my
            f = fx(i)*fy(j)
            dux2 = (u(i-1,j) - 2*u(i,j) + u(i+1,j))/(dx*dx)
            duy2 = (u(i,j-1) - 2*u(i,j) + u(i,j+1))/(dy*dy)
            lap = dux2 + duy2
            r(i,j) = f - lap
        end do
      end do

      end
