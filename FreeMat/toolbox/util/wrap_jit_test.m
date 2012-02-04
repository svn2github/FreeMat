function wrap_jit_test(name,speedtarget)
try
jcs = jitcontrol;
jitcontrol('off');
tic;
A1 = feval(name);
base = toc;
jitcontrol('on');
A2 = feval(name);
tic;
A2 = feval(name);
up = toc;
printf('Speedup %f\n',base/up);
if (issame(A1,A2) && (base/up > speedtarget))
  saveretvalue(0);
end
catch
end
