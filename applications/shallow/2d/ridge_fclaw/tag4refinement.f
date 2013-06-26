      subroutine ridge_tag4refinement(mx,my,mbc,meqn,
     &      xlower,ylower, dx,dy,q,maux,aux,initflag,tag_patch)
      implicit none

      integer mx,my, mbc, meqn, tag_patch, initflag, maux
      double precision xlower, ylower, dx, dy
      double precision q(1-mbc:mx+mbc,1-mbc:my+mbc,meqn)
      double precision aux(1-mbc:mx+mbc,1-mbc:my+mbc,maux)

      integer i,j, mq,m
      double precision xc,yc,s,eta
      double precision ampl, tol1, tol2, bmount

      common /cprob/ ampl, tol1, tol2

      tag_patch = 0
      do i = 1,mx
         do j = 1,my
            if (initflag .eq. 1) then
               tag_patch = 0
            else
               tag_patch = 1
            endif
c            eta = q(i,j,1) + aux(i,j,19)
c            if (abs(eta) .gt. tol1) then
c               tag_patch = 1
c               return
c            endif
         enddo
      enddo

      end

c     # We tag for coarsening if this coarsened patch isn't tagged for refinement
      subroutine ridge_tag4coarsening(mx,my,mbc,meqn,
     &      xlower,ylower,dx,dy,qcoarsened, maux,aux,initflag,tag_patch)
      implicit none

      integer mx,my, mbc, meqn, tag_patch, maux, initflag
      double precision xlower, ylower, dx, dy
      double precision qcoarsened(1-mbc:mx+mbc,1-mbc:my+mbc,meqn)
      double precision aux(1-mbc:mx+mbc,1-mbc:my+mbc,maux)

      integer i,j

      double precision ampl, tol1, tol2, eta
      common /cprob/ ampl, tol1, tol2


c     # The difference between this and the true "refinement" above is
c     # that we can't check ghost cells here.  Also, we may make the
c     # coarsening criteria different from the refinement criteria.
c     # Also, we don't check for an init_flag, since it is unlikely that
c     # we would coarsen an initial grid.

      tag_patch = 0
      do i = 1,mx
         do j = 1,my
            eta = qcoarsened(i,j,1) + aux(i,j,19)
            if (abs(eta) .gt. tol1) then
               tag_patch = 1
               return
            endif
         enddo
      enddo

      end