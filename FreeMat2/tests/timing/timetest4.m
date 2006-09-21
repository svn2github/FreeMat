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



function timetest4
  runcount = 10;
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
  
  for k = 1:numel(G)
    time = 0;
    for j = 1:runcount
      tic;
        eval(G{k});
        time = time + toc;
      end
      time = time/runcount;
      disp(sprintf('Run time for case %d is %f',k,time));
    end
    
      
      
      
  
