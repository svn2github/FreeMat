noinst_LIBRARIES = libFN.a

INCLUDES = -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libMex -I@top_srcdir@/libs/libXP

libFN_a_HFILES = FN.hpp \
LoadFN.hpp 

libFN_a_SOURCES = dpmpar.f \
enorm.f \
fdjac2.f \
lmdif1.f \
lmdif.f \
lmpar.f \
qrfac.f \
qrsolv.f \
FNFun.cpp \
Interp1D.cpp \
LoadFN.cpp \
OptFun.cpp \
$(libFN_a_HFILES)


AM_CPPFLAGS = $(QT_CFLAGS) 

AUTOMAKE_OPTIONS 	= foreign
