% LOGLOG LOGLOG Log-Log Plot Function
% 
% Usage
% 
% This command has the exact same syntax as the plot command:
% 
%   loglog(<data 1>,{linespec 1},<data 2>,{linespec 2}...,properties...)
% 
% in fact, it is a simple wrapper around plot that sets the
% x and y axis to have a logarithmic scale.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function ohandle = loglog(varargin)
    handle = plot(varargin{:});
    set(gca,'xscale','log');
    set(gca,'yscale','log');
    if (nargout > 0)
        ohandle = handle;
    end
    
