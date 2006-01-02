% Start with a regular sphere...
N = 37;
phi = repmat(linspace(0,pi,N),[N,1]);
theta = repmat(linspace(0,2*pi,N)',[1,N]);
zmat = cos(phi);
xmat = sin(phi).*cos(theta+phi);
ymat = sin(phi).*sin(theta+phi);
alphamat = ones(N);
alphamat(1:2:end,:) = 0;
surface('xdata',xmat,'ydata',ymat,'zdata',zmat,'alphadata',alphamat,'edgecolor','none','facealpha','flat');

