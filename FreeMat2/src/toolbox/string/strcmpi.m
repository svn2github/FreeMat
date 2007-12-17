%!
%@Module STRCMPI String Compare Case Insensitive Function
%@@Section STRING
%@@Usage
%Compares two strings for equality ignoring case.  The general
%syntax for its use is 
%@[
%   p = strcmpi(x,y)
%@]
%where @|x| and @|y| are two strings, or cell arrays of strings.
%See @|strcmp| for more help.
%@@Tests
%@$"y=strcmpi('astring','Astring')","1","exact"
%@$"y=strcmpi('astring','bstring')","0","exact"
%@$"x={'astring','bstring',43,'astring'};y=strcmpi(x,'asTring')","[1,0,0,1]","exact"
%@$"y=strcmpi({'this','is','a','pickle'},{'what','is','to','PIckle'})","[0,1,0,1]","exact"
%@$"y=strcmpi({'this','is','a','pickle'},['peter ';'piper ';'hated ';'picklE'])","[0,0,0,1]","exact"
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = strcmpi(source,pattern)
  y = strcmp(upper(source),upper(pattern));
  
