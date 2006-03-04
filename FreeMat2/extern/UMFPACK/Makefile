#-------------------------------------------------------------------------------
# UMFPACK makefile (for GNU make or original make)
#-------------------------------------------------------------------------------

# UMFPACK requires the AMD package to be in ../AMD

default: library

# compile all C code (except hb, fortran, and fortran64), including AMD and the
# MATLAB mexFunctions
all:
	( cd ../AMD ; make )
	( cd ../AMD/MATLAB ; make )
	( cd Source ; make )

# compile just the C-callable libraries and demo programs (not mexFunctions)
library:
	( cd ../AMD ; make )
	( cd Source ; make )

# compile the FORTRAN interface and demo program
fortran:
	( cd Demo   ; make fortran )

# compile the 64-bit FORTRAN interface and demo program
fortran64:
	( cd Demo   ; make fortran64 )

# compile the Harwell/Boeing demo program
hb:
	( cd Demo   ; make hb )

# remove object files, but keep the compiled programs and library archives
clean:
	( cd Source ; make purge )

# clean, and then remove compiled programs and library archives
purge:
	( cd ../AMD ; make purge )
	( cd Source ; make purge )

# create PDF documents for the original distribution
doc:
	( cd ../AMD ; make doc )
	( cd Doc    ; make )

# get ready for distribution
dist: purge
	( cd ../AMD ; make dist )
	( cd Demo   ; make dist )
	( cd Doc    ; make )

distclean: purge

ccode: library

# compile the MATLAB mexFunction
mex:
	( cd ../AMD/MATLAB ; make )
	( cd MATLAB ; make )
