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
  mperf = [0.010592 0.044388 0.055616 0.083225 0.778065 0.126407 0.756268 0.044205 0.119176 0.051550 0.231276 0.048534 0.083184 0.055793 0.083837 0.295581 0.323746 0.233305 0.132759];
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
        disp(sprintf('Run time for case %d is %f vs %f , %f objects copied',k,time,mperf(k),ogen));
      else
        disp(sprintf('Run time for case %d is %f',k,time));
      end
    end
