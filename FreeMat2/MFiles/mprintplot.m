function mprintplot(namebase)
epsname = sprintf('%s.eps',namebase);
printplot(epsname);
pngname = sprintf('%s.png',namebase);
printplot(pngname);
