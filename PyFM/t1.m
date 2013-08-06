function foo = bar(g,h,j)

a = 4 + ...
   5

%[a,b,c,d]=...
%goomba

a = 32;
if (x == 0)
  error 'hello'
else
  error 'goodby' 
end

x5=[-1;(1-sqrt(5))/2;0;(sqrt(5)-1)/2;1];

while a < b & c == 0
      err = foo;
      if err>reltol
	 h = h/2;
      else
         k = k + 1;
      end
end

while t(k)<tspan(2) & terminate==0
    [yn,error,fs,interp,pwr]=feval(stepper,t(k),y(:,k),h,f0,f,varargin);
    err=h*max(abs(error)./max(max(abs(y(:,k)),abs(yn)),abstol/reltol));
    if err>reltol
       h=h/2;
    else
       k=k+1;
       y(:,k)=yn;
       t(k)=t(k-1)+h;
       if size(fs,2)==6 fs(:,7)=feval(f,t(k),yn,varargin{:}); end;
       f0=fs(:,7);
       f3d(k,:,:)=fs;
       if ~ isempty(event)
%         [ts,ys,ws,terminate]= ...
%               findallzeros(t(k-1),t(k),y(:,k-1),feval(interp,y(:,k-1),yn,h,fs),...
%               abstol,event,varargin);
%           te=[te;ts]; ye=[ye,ys]; we=[we;ws];
%	 4;
%	  [a,b,c,d]= ...
%	       foobar
      end;
   end
end
