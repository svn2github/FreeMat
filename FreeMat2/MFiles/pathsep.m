
function x = pathsep
if (strcmp(computer,'PCWIN'))
  x = ';';
else
  x = ':';
end
