function ft = timetest3
%  fp = fopen('mat.dat','rb');
%  ijv = fread(fp,[2984983,3],'double');
%  A = sparse(ijv(:,1),ijv(:,2),ijv(:,3),30000,10000);
%  A = sprandn(30000,10000,0.01);
  A = randn(3000,2000);
  rall = 1:3000;
  rsub = rall; rsub(2500) = [];
  call = 1:2000;
  csub = call; csub(500) = [];
  ft = [];
  printf('Starting time trial...\n');
  ft(1) = timeit('b = A(:,346);');
  ft(3) = timeit('b = A(rall,346);');
  ft(5) = timeit('b = A(rsub,346);');
  ft(7) = timeit('b = A(346,:);');
  ft(9) = timeit('b = A(346,call);');
  ft(11) = timeit('b = A(346,csub);');
  ft(13) = timeit('b = A(346:366,:);');
  ft(15) = timeit('b = A(346:366,call);');
  ft(17) = timeit('b = A(346:366,csub);');
  return
  ft(2) = timeit('A(:,346) = b;');
  ft(4) = timeit('A(rall,346) = b;');
  ft(6) = timeit('A(rsub,346) = b;');
  ft(8) = timeit('A(346,:) = b;');
  ft(10) = timeit('A(346,call) = b;');
  ft(12) = timeit('A(346,csub) = b;');
  ft(14) = timeit('A(346:366,:) = b;');
  ft(16) = timeit('A(346:366,call) = b;');
  ft(18) = timeit('A(346:366,csub) = b;');
  
function t = timeit(expr)
  printf('Testing %s\n',expr);
  tic;
    evalin('caller',['for i=1:1000; ' expr ' end;']);
  t = toc;
  
  
function printf(varargin)
  disp(sprintf(varargin{:}));
  
