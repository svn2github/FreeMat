
function y = input(prompt,stringflag)
  if (isset('stringflag') && ischar(stringflag) && (strcmp(stringflag,'s') | strcmp(stringflag,'S')))
    y = getline(prompt);
    y(end) = [];
  elseif isset('stringflag')
    error 'second argument to input must be the string ''s'' or ''S''.'
  else
    needval = 1;
    while (needval)
      a = getline(prompt);
      a(end) = [];
      needval = 0;
      y = evalin('caller',a,'0;printf(''%s\n'',lasterr);needval=1;');
    end
  end
