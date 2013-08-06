function [a,b,c] = foo(d,e,f)
dbstep
global a b c 
persistent q e d
  a = 32;
b = a + 52 - c;
end
