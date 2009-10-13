% REHASH REHASH Rehash Directory Caches
% 
% Usage
% 
% Usually, FreeMat will automatically determine when M Files have
% changed, and pick up changes you have made to M files.  Sometimes,
% you have to force a refresh.  Use the rehash command for this
% purpose.  The syntax for its use is 
% 
%   rehash
% 

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function rehash
  cd(pwd);
