t = (0:100)/100*2*pi;
plot(t,cos(t),'r:');
xlabel('Hello');
ylabel('Gooboo');
title('Evolution');
if (t(2) > 0)
  s = 342;
  t = auxfunc(3,4);
  keyboard
  k = 0;
  while (k<100)
    k = k + 1;
  end
  keyboard
  printf('Hello!\n');
end
