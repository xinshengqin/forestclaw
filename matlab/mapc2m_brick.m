function [xp,yp,zp] = mapc2m_brick(xc,yc,s)

% This assumes that (xc,yc) is in [0,1]x[0,1]. The blockno then
% tells us how to map to the correct brick position

if (nargin < 3)
   s = 0;
end

% This adds some space between blocks.
% xc = (1-s)*(xc - 0.5) + 0.5;
% yc = (1-s)*(yc - 0.5) + 0.5;
xc = (1-s)*xc + s/2;
yc = (1-s)*yc + s/2;

brick_data = load('brick.dat');
mi = brick_data(1,1);
mj = brick_data(1,2);
xv = brick_data(2:end,1);
yv = brick_data(2:end,2);

blockno = getblocknumber();

xp = (xv(blockno+1) + xc)/mi;
yp = (yv(blockno+1) + yc)/mj;
zp = 0*xp;

end