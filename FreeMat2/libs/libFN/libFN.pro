TEMPLATE = lib

TARGET = FN

CONFIG += staticlib

INCLUDEPATH = ../libFreeMat

HEADERS += LoadFN.hpp \
FN.hpp

SOURCES += OptFun.cpp \
LoadFN.cpp \
Interp1D.cpp  \
FNFun.cpp \
lmdif1.f lmdif.f  lmpar.f  qrfac.f \
dpmpar.f enorm.f qrsolv.f fdjac2.f \
erf.f derf.f ei.f dei.f daw.f ddaw.f \
psi.f dpsi.f gamma.f dgamma.f algama.f dlgama.f 

DEPENDPATH += INCLUDEPATH
