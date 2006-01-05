function h = tube(x,y,z,radius,thetcount)
  x = x(:);
  y = y(:);
  z = z(:);
  if ((length(x) ~= length(y)) | (length(x) ~= length(z)))
    error('Tube requires all arguments to be the same size');
  end
  N = length(x);
  grad = [x(2)-x(1),y(2)-y(1),z(2)-z(1)];
  v = nullspace(grad);
  thetas = linspace(0,2*pi,thetcount);
  ptx = x(1) + radius*(cos(thetas)*v(1,1) + sin(thetas)*v(1,2));
  pty = y(1) + radius*(cos(thetas)*v(2,1) + sin(thetas)*v(2,2));
  ptz = z(1) + radius*(cos(thetas)*v(3,1) + sin(thetas)*v(3,2));
  sx = [ptx(:)']; sy = [pty(:)']; sz = [ptz(:)'];
  for i=2:(N-1)
    grad = [x(i+1)-x(i),y(i+1)-y(i),z(i+1)-z(i)];
    ptx = ptx + grad(1);
    pty = pty + grad(2);
    ptz = ptz + grad(3);
    v = nullspace(grad);
    b = v*(v'*[ptx-x(i);pty-y(i);ptz-z(i)]);
    b = b./repmat(sqrt(sum(b.^2)),[3,1])*radius;
    ptx = x(i) + b(1,:);
    pty = y(i) + b(2,:);
    ptz = z(i) + b(3,:);
    sx = [sx;ptx]; sy = [sy;pty]; sz = [sz;ptz];
  end
  h = surface('xdata',sx,'ydata',sy,'zdata',sz);

function v = nullspace(grad)
  A = [grad;grad;grad];
  [u,s,v] = svd(A);
  v = v(:,2:3);
  
 