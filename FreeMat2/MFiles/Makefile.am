EXTRA_DIST = clusterboot.m\
copper.m\
exit.m\
eye.m\
fliplr.m\
flipud.m\
gray.m\
imagez.m\
length.m\
linspace.m\
log2.m\
mprintimage.m\
mprintplot.m\
popdir.m\
pushdir.m\
rawread.m\
rcall.m\
rescan.m\
reshape.m\
rserver.m\
sqrt.m\
std.m\
winlev.m

install-data-local:
	mkdir -p $(prefix)/share/FreeMat/MFiles
	for scriptname in $(EXTRA_DIST); do \
	  echo "Installing M File $$scriptname"; \
	  $(INSTALL) $(srcdir)/$$scriptname $(prefix)/share/FreeMat/MFiles;\
	done;
