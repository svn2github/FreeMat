function timetest3
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
  vprintf('Starting time trial...\n');
  timeit('b = A(:,346);');
  timeit('A(:,346) = b;');
  timeit('b = A(rall,346);');
  timeit('A(rall,346) = b;');
  timeit('b = A(rsub,346);');
  timeit('A(rsub,346) = b;');  
  timeit('b = A(346,:);');
  timeit('A(346,:) = b;');  
  timeit('b = A(346,call);');
  timeit('A(346,call) = b;');
  timeit('b = A(346,csub);');
  timeit('A(346,csub) = b;');
  timeit('b = A(346:366,:);');
  timeit('A(346:366,:) = b;');
  timeit('b = A(346:366,call);');
  timeit('A(346:366,call) = b;');
  timeit('b = A(346:366,csub);');
  timeit('A(346:366,csub) = b;');
  
function t = timeit(expr)
  tic;
    evalin('caller',['for i=1:1000; ' expr ' end;']);
  t = toc;
  vprintf('Time trial %s is %f second\n',expr,t);
  
function vprintf(varargin)
  if (exist('strcomp'))
    printf(varargin{:});
  else
    disp(sprintf(varargin{:}));
  end
  
