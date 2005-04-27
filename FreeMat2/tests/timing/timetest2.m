function ft = timetest2
  fp = fopen('mat.dat','rb');
  ijv = fread(fp,[2984983,3],'double');
  A = sparse(ijv(:,1),ijv(:,2),ijv(:,3),30000,10000);
%  A = sprandn(30000,10000,0.01);
  rall = 1:30000;
  rsub = rall; rsub(25000) = [];
  call = 1:10000;
  csub = call; csub(5000) = [];
  ft = [];
  printf('Starting time trial...\n');
  ft(1) = timeit('b = A(:,346);');
  ft(2) = timeit('b = A(rall,346);');
  ft(3) = timeit('b = A(rsub,346);');
%  ft(4) = timeit('b = A'';');
  ft(5) = timeit('b = A(346,:);');
  ft(6) = timeit('b = A(346,call);');
  ft(7) = timeit('b = A(346,csub);');
  ft(8) = timeit('b = A(346:366,:);');
  ft(9) = timeit('b = A(346:366,call);');
  ft(10) = timeit('b = A(346:366,csub);');
%    t(5) = timeit('a = full(A);');
%    t(6) = timeit('b = a(:,346);');
%    t(7) = timeit('b = a(pall,346);');
%    t(8) = timeit('b = a(psub,346);');
%    t(9) = timeit('b = a'';');
  
function t = timeit(expr)
  printf('Testing %s\n',expr);
  tic;
    evalin('caller',['for i=1:50; ' expr ' end;']);
  t = toc;
  
  
function printf(varargin)
  disp(sprintf(varargin{:}));
  
