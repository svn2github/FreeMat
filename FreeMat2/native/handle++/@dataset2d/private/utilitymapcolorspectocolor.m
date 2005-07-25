function color = utilitymapcolorspectocolor(cspec) {
    switch(cspec) 
     case {'y'}
    color = [255,255,0];
     case {'m'}
    color = [255,0,255];
     case {'c'}
    color = [0,255,255];
     case {'r'}
    color = [255,0,0];
     case {'g'}
    color = [0,255,0];
     case {'b'}
    color = [0,0,255];
     case {'w'}
    color = [255,255,255];
     case {'k'}
    color = [0,0,0];
     otherwise:
    color = [0,0,0];
    end
    
