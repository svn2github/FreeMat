EXTRA_DIST = gfitfun.m \
std.m \
sqrt.m \
rserver.m \
reshape.m \
rescan.m \
rcall.m \
rawread.m \
pushdir.m \
popdir.m \
log2.m \
linspace.m \
length.m \
imagez.m \
gray.m \
flipud.m \
fliplr.m \
eye.m \
exit.m \
copper.m \
clusterboot.m \
mprintplot.m \
mprintimage.m \
mpispawn.m \
mpiserver.m \
mpirun.m \
bind.m \
gfitfun.m \
gausfit.m

install-data-local:
	mkdir -p $(prefix)/share/FreeMat/MFiles
	for scriptname in $(EXTRA_DIST); do \
	  echo "Installing M File $$scriptname"; \
	  $(INSTALL) $(srcdir)/$$scriptname $(prefix)/share/FreeMat/MFiles;\
	done;
uninstall-local:
	for scriptname in $(EXTRA_DIST); do \
	  echo "Uninstalling M File $$scriptname"; \
	  rm -f $(prefix)/share/FreeMat/MFiles/$$scriptname;\
	done;

