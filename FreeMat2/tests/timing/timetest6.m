%--> timetest4
% Run time for case 1 is 0.011690 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.058184 vs 0.044388 , 11.100000 objects copied
% Run time for case 3 is 0.074495 vs 0.055616 , 11.200000 objects copied
% Run time for case 4 is 0.465126 vs 0.083225 , 100011.100000 objects copied
% Run time for case 5 is 0.820692 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.219805 vs 0.126407 , 100014.000000 objects copied
% Run time for case 7 is 0.373643 vs 0.756268 , 100009.200000 objects copied
% Run time for case 8 is 0.058302 vs 0.044205 , 11.200000 objects copied
% Run time for case 9 is 0.081880 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.190178 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.491384 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.282191 vs 0.048534 , 100013.100000 objects copied
% Run time for case 13 is 0.462326 vs 0.083184 , 100012.200000 objects copied
% Run time for case 14 is 0.075529 vs 0.055793 , 12.200000 objects copied
% Run time for case 15 is 0.461496 vs 0.083837 , 100013.100000 objects copied
% Run time for case 16 is 0.508565 vs 0.295581 , 100015.100000 objects copied
% Run time for case 17 is 0.708904 vs 0.323746 , 200016.100000 objects copied
% Run time for case 18 is 0.526946 vs 0.233305 , 100016.200000 objects copied
%
%
%
% I want to optimize 
%  X(i,j) = expression
% If X appears on the right hand side, we have a bit of a problem.
%
% So I have concluded:
%   1. Ref counting is OK
%   2. Assignment statements need to be improved
%   3. Expressions need a destination (generally)
%   4. In (e.g.) an Add expression, we have to check
%
%   Add(A, B, *C)
% 
%   If (A == C) or (B == C), then we really want an Increment operator.
%
%   Should this be detected a level up in the code?
%
%   I.e., can we simply do:
%
%   M = M + 1 --> Inc(M)
% 
%   After all, I suspect that M = 3+4-(7+M) does not map to Inc(M)
%
%   Besides, consider:
%
%    M = (3 + M) + (5 - M)
%  
%   Here we _have_ to make a copy of M. So really the ref counting scheme
%   is fine.  We just need to do some work to recognize
%   increment/decrement and other operators.
%
% Run time for case 1 is 0.012268 vs 0.010592: ratio 115.827983 , 5.300000 objects                  % Run time for case 2 is 0.062235 vs 0.044388: ratio 140.206587 , 5.700000 objects               
% Run time for case 3 is 0.078400 vs 0.055616: ratio 140.966988 , 5.800000 objects        
% Run time for case 4 is 0.296711 vs 0.083225: ratio 356.516431 , 5.700000 objects            
% Run time for case 5 is 0.795687 vs 0.778065: ratio 102.264901 , 200006.600000 objects copied
% Run time for case 6 is 0.158514 vs 0.126407: ratio 125.400097 , 100007.700000 objects copied
% Run time for case 7 is 0.267853 vs 0.756268: ratio 35.417722 , 100003.800000 objects copied
% Run time for case 8 is 0.063504 vs 0.044205: ratio 143.659088 , 5.800000 objects copied
% Run time for case 9 is 0.083698 vs 0.119176: ratio 70.230667 , 5.700000 objects copied
% Run time for case 10 is 0.186392 vs 0.051550: ratio 361.574588 , 6.800000 objects copied
% Run time for case 11 is 0.325240 vs 0.231276: ratio 140.628297 , 6.800000 objects copied
% Run time for case 12 is 0.163408 vs 0.048534: ratio 336.686859 , 100001.100000 objects copied
% Run time for case 13 is 0.296757 vs 0.083184: ratio 356.747932 , 6.600000 objects copied
% Run time for case 14 is 0.078616 vs 0.055793: ratio 140.907282 , 6.800000 objects copied
% Run time for case 15 is 0.295883 vs 0.083837: ratio 352.926631 , 7.700000 objects copied
% Run time for case 16 is 0.316530 vs 0.295581: ratio 107.087533 , 9.800000 objects copied
% Run time for case 17 is 0.397389 vs 0.323746: ratio 122.747123 , 100003.400000 objects copied
% Run time for case 18 is 0.391285 vs 0.233305: ratio 167.713894 , 100009.700000 objects copied
% Run time for case 19 is 0.705349 vs 0.132759: ratio 531.300401 , 9.700000 objects copied
% Run time for case 20 is 0.294386 vs 0.081160: ratio 362.723632 , 6.700000 objects copied
% Run time for case 21 is 0.294650 vs 0.080921: ratio 364.120809 , 6.700000 objects copied
% Run time for case 22 is 0.503747 vs 0.106084: ratio 474.856435 , 7.700000 objects copied
% Run time for case 23 is 0.566735 vs 0.111576: ratio 507.936026 , 7.700000 objects copied
function timetest6(countflag)
  runcount = 10;
  if (~exist('countflag')) countflag = 1; end
  G{1} = 'for i=1:100000; end;';
  G{2} = 'for i=1:100000; i = 2; end;';
  G{3} = 'm = 2; for i=1:100000; i = m; end;';
  G{4} = 'for i=1:100000; i = i + 1; end;';
  G{5} = 'for i=1:100000; i = [1,1]; end;';
  G{6} = 'm = [1,1]; for i=1:100000; i = m; end;';
%  G{7} = 'for i=1:100000; clear i; end;';
  G{7} = 'for i=1:100000;  i; end;';
  G{8} = 'for i=1:100000; m = 2; end;';
  G{9} = 'for i=1:100000; 2; end;';
  G{10} = 'm = [1,1]; for i=1:100000; m; end;';
  G{11} = 'm = [1,1]; for i=1:100000; n = m + m; end;';
  G{12} = 'm = [1,1]; for i=1:100000; m = m; end;';
  G{13} = 'm = 0; for i=1:100000; m = m + 1; end;';
  G{14} = 'm = 3; n = 0; for i=1:100000; n = m; end;';
  G{15} = 'm = 3; n = 0; for i=1:100000; n = m+1; end;';
  G{16} = 'm = [1,1]; n = 0; for i=1:100000; n = m+1; end;';
  G{17} = 'm = zeros(2,2,2); n = 0; for i=1:100000; n = m+1; end;';
  G{18} = 'm = 3; n = 0; g = [1,1]; for i=1:100000; n = g; n = m+1; end;';
  G{19} = 'm = 0; for i=1:100000; m = 4 + 3 - (m + 1); end;';
  G{20} = 'm = 0; for i=1:100000; m = 1 + m; end;';
  G{21} = 'm = 0; for i=1:100000; m = 5 + m; end;';
  G{22} = 'm = 0; for i=1:100000; m = 1 + 1 + m; end;';
  G{23} = 'm = 0; for i=1:100000; m = 1*m + 1; end;';
  mperf = [0.010592 %1
           0.044388 %2
           0.055616 %3
           0.083225 %4
           0.778065 %5
           0.126407 %6
           0.756268 %7
           0.044205 %8
           0.119176 %9
           0.051550 %10
           0.231276 %11
           0.048534 %12
           0.083184 %13
           0.055793 %14
           0.083837 %15
           0.295581 %16
           0.323746 %17
           0.233305 %18
           0.132759 %19
           0.081160 %20
           0.080921 %21
           0.106084 %22
           0.111576 ]; %23];
  for k = 1:numel(G)
    time = 0;
    ogen = 0;
    for j = 1:runcount
      if (countflag), ocount = demo(16); end;
      tic;
        eval(G{k});
        time = time + toc;
        if (countflag), ogen = ogen + demo(16) - ocount; end;
      end
      time = time/runcount;
      if (countflag), ogen = ogen/runcount; end;
      if (countflag)
        disp(sprintf('Run time for case %d is %f vs %f: ratio %f , %f objects copied',k,time,mperf(k),time/mperf(k)*100,ogen));
      else
        disp(sprintf('Run time for case %d is %f',k,time));
      end
    end
