%
% function ret = rcall(address,&handle,fname,varargin)
%
% Call the function fname on machine with address address, with
% the given arguments.  If handle is 0, a connection to the
% server is made on port 43000, and handle is modified.

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
function ret = rcall(address,&handle,fname,varargin)
if handle == 0
  handle = connect(address,43000);
end
send(handle,fname);
printf('sent function name %s\n',fname);
argcount = length(varargin);
send(handle,argcount);
printf('sent arg count %d\n',argcount);
for j=1:argcount
  send(handle,varargin{j});
  printf('sent arg %d\n',j);
end
printf('waiting for reply...',j);
rtype = receive(handle);
printf('reply is %s\n',rtype);
if (rtype == 's')
  ret = receive(handle);
  return
elseif (rtype == 'e')
  ret = receive(handle);
  error(ret);
else
  ret = [];
end

