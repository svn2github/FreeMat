%>> timetest4
%Run time for case 1 is 0.010479 ok
%Run time for case 2 is 0.043684 
%Run time for case 3 is 0.055329
%Run time for case 4 is 0.082830
%Run time for case 5 is 0.784426 ok
%Run time for case 6 is 0.124472 
%Run time for case 7 is 0.738984
%Run time for case 8 is 0.047794
%Run time for case 9 is 0.115748 ok
%Run time for case 10 is 0.051994
%Run time for case 11 is 0.224738
%Run time for case 12 is 0.083184
%Run time for case 13 is 0.055793
%
% FreeMat 2.0
%--> timetest4
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
%Error: Expecting identifier or assignment (id = expr) after 'for'
% Run time for case 1 is 0.002028
% Run time for case 2 is 2.191146
% Run time for case 3 is 2.220203
% Run time for case 4 is 2.640038
% Run time for case 5 is 2.701126
% Run time for case 6 is 2.207211
% Run time for case 7 is 0.000087
% Run time for case 8 is 2.197576
% Run time for case 9 is 2.196498
%
% FreeMat
%--> timetest4
% Run time for case 1 is 0.016014
% Run time for case 2 is 0.257976
% Run time for case 3 is 0.387730
% Run time for case 4 is 0.768118
% Run time for case 5 is 1.156680
% Run time for case 6 is 0.340649
% Run time for case 7 is 2.346052
% Run time for case 8 is 0.275053
%
% FreeMat 2.1 - ForLoopHelper and literals pre-instancing.
%--> timetest4
% Run time for case 1 is 0.001104
% Run time for case 2 is 0.272548
% Run time for case 3 is 0.388887
% Run time for case 4 is 0.596402
% Run time for case 5 is 0.809294
% Run time for case 6 is 0.336881
% Run time for case 7 is 2.396140
% Run time for case 8 is 0.090501
% Run time for case 9 is 0.124992
% Run time for case 10 is 0.286669
% Run time for case 11 is 0.706027
%
% Octave (test 7 hangs)
% 
%octave:2> timetest4
%Run time for case 1 is 0.020229
%Run time for case 2 is 0.156994
%Run time for case 3 is 0.203542
%Run time for case 4 is 0.245667
%Run time for case 5 is 0.157356
%Run time for case 6 is 0.203923
%Run time for case 7 is 0.000379
%Run time for case 8 is 0.155819
%Run time for case 9 is 0.128124
%octave:3>


% Looking at test #10, I don't see why this is at 268 ms.  A simple
% look at demo(17) indicates that the lookup time for a variable
% should be no more than 30ms, and there are copies being made.
% Commenting out the call to rhsExpression changes the runtime to
% 
% Here is the current summary of performance:
%--> timetest4(1)
% Run time for case 1 is 0.016765 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.278605 vs 0.044388 , 100010.100000 objects copied
% Run time for case 3 is 0.359015 vs 0.055616 , 100010.100000 objects copied
% Run time for case 4 is 0.558223 vs 0.083225 , 100011.200000 objects copied
% Run time for case 5 is 0.802652 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.305044 vs 0.126407 , 100013.100000 objects copied
% Run time for case 7 is 2.290032 vs 0.756268 , 200010.100000 objects copied
% Run time for case 8 is 0.089313 vs 0.044205 , 11.200000 objects copied
% Run time for case 9 is 0.120596 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.270645 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.631564 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.552238 vs 0.083184 , 100012.100000 objects copied
% Run time for case 13 is 0.165995 vs 0.055793 , 12.100000 objects copied
%
% Case 13 is interesting.  No data copies are made, but it is still 3X too slow.
% One possible culprit is the mutex.  Commenting out the QMutexLocker yields:
% Run time for case 1 is 0.016167 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.251691 vs 0.044388 , 100010.100000 objects copied
% Run time for case 3 is 0.413307 vs 0.055616 , 100010.100000 objects copied
% Run time for case 4 is 0.658801 vs 0.083225 , 100011.200000 objects copied
% Run time for case 5 is 0.781440 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.273061 vs 0.126407 , 100013.100000 objects copied
% Run time for case 7 is 2.171303 vs 0.756268 , 200010.100000 objects copied
% Run time for case 8 is 0.083725 vs 0.044205 , 11.200000 objects copied
% Run time for case 9 is 0.106028 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.214963 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.711335 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.657318 vs 0.083184 , 100012.100000 objects copied
% Run time for case 13 is 0.141301 vs 0.055793 , 12.100000 objects copied
%
% Demo18 does a simple j=i assignment inside a loop, via the Context interface.
% its run time is 65 ms.
% Demo19 uses the scope interface (bypassing Context).  Its run time is 55 msec.
%
% One way to get this type of performance is to have the parser annotate the 
% variables at parse time. But I still need to understand the 2X penalty for
% case 13 between what I think should happen and what appears to be happening.
%
% Demo20 uses the context interface, but includes the ArrayVector routing.
% This has a penalty, and brings the total up to about 92 msec.
%
% Special casing the lookup of identifiers in Interpreter::expression() yields the
% following run times:
%--> timetest4
% Run time for case 1 is 0.016563 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.258637 vs 0.044388 , 100010.100000 objects copied
% Run time for case 3 is 0.276692 vs 0.055616 , 100010.100000 objects copied
% Run time for case 4 is 0.476320 vs 0.083225 , 100011.200000 objects copied
% Run time for case 5 is 0.803687 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.224013 vs 0.126407 , 100013.100000 objects copied
% Run time for case 7 is 2.152764 vs 0.756268 , 200010.100000 objects copied
% Run time for case 8 is 0.073595 vs 0.044205 , 11.200000 objects copied
% Run time for case 9 is 0.110448 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.219278 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.503066 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.489793 vs 0.083184 , 100012.100000 objects copied
% Run time for case 13 is 0.089882 vs 0.055793 , 12.100000 objects copied
%-->
%This remaining ~30ms penalty could be due to the trip through Context instead of Scope.
%to try to eliminate this penalty, I kept track of the top and bottom scopes
%in local variables in Context (to avoid the repeated calls to back() and
%front() on the scope stack).  
%
%%After this change, the times are:
%--> timetest4
% Run time for case 1 is 0.016548 vs 0.010592 , 10.200000 objects copied
% Run time for case 2 is 0.259969 vs 0.044388 , 100010.100000 objects copied
% Run time for case 3 is 0.276733 vs 0.055616 , 100010.100000 objects copied
% Run time for case 4 is 0.483276 vs 0.083225 , 100011.200000 objects copied
% Run time for case 5 is 0.825061 vs 0.778065 , 200012.000000 objects copied
% Run time for case 6 is 0.227777 vs 0.126407 , 100013.100000 objects copied
% Run time for case 7 is 2.204037 vs 0.756268 , 200010.100000 objects copied
% Run time for case 8 is 0.085292 vs 0.044205 , 11.200000 objects copied
% Run time for case 9 is 0.105501 vs 0.119176 , 11.100000 objects copied
% Run time for case 10 is 0.256824 vs 0.051550 , 13.100000 objects copied
% Run time for case 11 is 0.501826 vs 0.231276 , 100013.100000 objects copied
% Run time for case 12 is 0.488088 vs 0.083184 , 100012.100000 objects copied
% Run time for case 13 is 0.095693 vs 0.055793 , 12.100000 objects copied
%
% Demo21 measures the time associated with an Array->Array copy.  It clocks
% in at about 3ms for a copy.  Not too bad.  So what is the discrepancy between
% the expression "a=b" and Demo18 (65 ms vs 95 ms)?  
function timetest4(countflag)
  runcount = 10;
  if (~exist('countflag')) countflag = 1; end
  G{1} = 'for i=1:100000; end;';
  G{2} = 'for i=1:100000; i = 2; end;';
  G{3} = 'm = 2; for i=1:100000; i = m; end;';
  G{4} = 'for i=1:100000; i = i + 1; end;';
  G{5} = 'for i=1:100000; i = [1,1]; end;';
  G{6} = 'm = [1,1]; for i=1:100000; i = m; end;';
  G{7} = 'for i=1:100000; clear i; end;';
  G{8} = 'for i=1:100000; m = 2; end;';
  G{9} = 'for i=1:100000; 2; end;';
  G{10} = 'm = [1,1]; for i=1:100000; m; end;';
  G{11} = 'm = [1,1]; for i=1:100000; n = m + m; end;';
  G{12} = 'm = [1,1]; for i=1:100000; m = m; end;';
  G{13} = 'm = 0; for i=1:100000; m = m + 1; end;';
  G{14} = 'm = 3; n = 0; for i=1:100000; n = m; end;';
  
  mperf = [0.010592 0.044388 0.055616 0.083225 0.778065 0.126407 0.756268 0.044205 0.119176 0.051550 0.231276 0.048534 0.083184 0.055793];
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
    
      
      
      
  
