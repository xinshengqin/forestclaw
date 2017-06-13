	subroutine amg_fill_patch(mx,my,mbc,q,u)
	implicit none

	integer mx,my,mbc
	double precision q(mx,my)
	double precision u(1-mbc:mx+mbc,1-mbc:my+mbc)

	integer i,j

	do i = 1,mx
		do j = 1,my
			u(i,j) = q(i,j)
		end do
	end do

	end
