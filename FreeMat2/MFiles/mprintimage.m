function mprintimage(namebase)
epsname = sprintf('%s.eps',namebase);
printimage(epsname);
pngname = sprintf('%s.png',namebase);
printimage(pngname);
