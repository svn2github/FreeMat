set(FN_SRCS  OptFun.cpp LoadFN.cpp
Interp1D.cpp FNFun.cpp lmdif1.f
lmdif.f  
lmpar.f qrfac.f dpmpar.f
enorm.f qrsolv.f fdjac2.f
erf.f derf.f ei.f dei.f
daw.f ddaw.f psi.f dpsi.f
gamma.f dgamma.f algama.f
dlgama.f ) 

# build sources, moc'd sources, and rcc'd sources
add_library(FN STATIC ${FN_SRCS} )
target_link_libraries ( FN FM ${FORT_LIBRARY}) 

SET_TARGET_PROPERTIES( FN PROPERTIES LINKER_LANGUAGE CXX )
