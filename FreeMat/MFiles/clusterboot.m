%
% function handle = clusterboot(nodes,serverip,rshcmd,graphicsflag)
%
% Start up the cluster by spawning a FreeMat session on each node
% in a cluster.  The nodes argument is a cell array of IP addresses/
% hostnames.  The serverip is the ip address of the server (the
% IP address of the machine that is running).  The rshcmd argument 
% is the command to execute for each node.  If graphicsflag is 1, 
% then the spawned processes are graphics enabled, if graphicsflag 
% is 0, then the spawned processes are not graphics enabled.
%
% Example:
%
% Suppose that the nodes are named 'gn0','gn1',... and we 
% want to use ssh to establish connections to each of them.  Then
% the following commands:
%
% nodes = {'gn0','gn1','gn2','gn3');
% handle = clusterboot(nodes,'10.0.0.1','ssh',0);
%

% Copyright (c) 2002, 2003 Samit Basu
%
% Permission is hereby granted, free of charge, to any person obtaining a 
% copy of this software and associated documentation files (the "Software"), 
% to deal in the Software without restriction, including without limitation 
% the rights to use, copy, modify, merge, publish, distribute, sublicense, 
% and/or sell copies of the Software, and to permit persons to whom the 
% Software is furnished to do so, subject to the following conditions:
%
% The above copyright notice and this permission notice shall be included 
% in all copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
% OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
% THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
% FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
% DEALINGS IN THE SOFTWARE.
function cdata = clusterboot(nodes,serverip,rshcmd,graphicsflag)
  % First, open a server socket on an arbitrary port
  [cdata.handle,portnum] = server(0);
  % Now, loop over the node names
  for i=1:length(nodes)
    % Create the command to be executed
    if (graphicsflag)
      cmd = sprintf('%s FreeMat -e clusterserver %s %d &',rshcmd,serverip,portnum);
    else
      cmd = sprintf('%s FMKernel -e clusterserver %s %d &',rshcmd,serverip,portnum);
    end
    printf('Executing %s\n',cmd);
    system(cmd);
    % Get a connection from each node
    printf('Waiting for connection...');
    cdata.client(i) = accept(cdata.handle);
    printf('done\n');
  end
