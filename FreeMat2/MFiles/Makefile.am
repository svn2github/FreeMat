EXTRA_DIST = bind.m\
circshift.m\
copper.m\
exit.m\
eye.m\
filesep.m\
flipdim.m\
fliplr.m\
flipud.m\
gausfit.m\
gfitfun.m\
gray.m\
idiv.m\
isa.m\
length.m\
linspace.m\
log10.m\
log2.m\
mod.m\
mpirun.m\
mpiserver.m\
mprintimage.m\
mprintplot.m\
nonzeros.m\
norm.m\
numel.m\
path.m\
pathsep.m\
pinv.m\
polyfit.m\
polyval.m\
popdir.m\
pushdir.m\
rank.m\
rawread.m\
rescan.m\
reshape.m\
speye.m\
sprand.m\
sprandn.m\
spy.m\
sqrt.m\
squeeze.m\
std.m\
teps.m\
xlim.m\
ylim.m

AUTOMAKE_OPTIONS = foreign

install-data-local:
	mkdir -p $(prefix)/share/FreeMat/MFiles
	for scriptname in $(EXTRA_DIST); do \
	  echo "Installing M file $$scriptname"; \
	  $(INSTALL) $(srcdir)/$$scriptname $(prefix)/share/FreeMat/MFiles;\
	done;
uninstall-local:
	for scriptname in $(EXTRA_DIST); do \
	  echo "Uninstalling M file $$scriptname"; \
	  rm -f $(prefix)/share/FreeMat/MFiles/$$scriptname;\
	done;
