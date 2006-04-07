set(FN_SRCS  OptFun.cpp LoadFN.cpp
Interp1D.cpp FNFun.cpp
lmdif1.o 
lmdif.o 
lmpar.o 
qrfac.o 
dpmpar.o 
enorm.o 
qrsolv.o 
fdjac2.o 
erf.o 
derf.o 
ei.o 
dei.o 
daw.o 
ddaw.o 
psi.o 
dpsi.o 
gamma.o 
dgamma.o 
algama.o 
dlgama.o )

add_custom_command(OUTPUT lmdif1.o COMMAND gfortran -c lmdif1.f)
add_custom_command(OUTPUT lmdif.o COMMAND gfortran -c lmdif.f)
add_custom_command(OUTPUT lmpar.o COMMAND gfortran -c lmpar.f)
add_custom_command(OUTPUT qrfac.o COMMAND gfortran -c qrfac.f)
add_custom_command(OUTPUT dpmpar.o COMMAND gfortran -c dpmpar.f)
add_custom_command(OUTPUT enorm.o COMMAND gfortran -c enorm.f)
add_custom_command(OUTPUT qrsolv.o COMMAND gfortran -c qrsolv.f)
add_custom_command(OUTPUT fdjac2.o COMMAND gfortran -c fdjac2.f)
add_custom_command(OUTPUT erf.o COMMAND gfortran -c erf.f)
add_custom_command(OUTPUT derf.o COMMAND gfortran -c derf.f)
add_custom_command(OUTPUT ei.o COMMAND gfortran -c ei.f)
add_custom_command(OUTPUT dei.o COMMAND gfortran -c dei.f)
add_custom_command(OUTPUT daw.o COMMAND gfortran -c daw.f)
add_custom_command(OUTPUT ddaw.o COMMAND gfortran -c ddaw.f)
add_custom_command(OUTPUT psi.o COMMAND gfortran -c psi.f)
add_custom_command(OUTPUT dpsi.o COMMAND gfortran -c dpsi.f)
add_custom_command(OUTPUT gamma.o COMMAND gfortran -c gamma.f)
add_custom_command(OUTPUT dgamma.o COMMAND gfortran -c dgamma.f)
add_custom_command(OUTPUT algama.o COMMAND gfortran -c algama.f)
add_custom_command(OUTPUT dlgama.o COMMAND gfortran -c dlgama.f)

# build sources, moc'd sources, and rcc'd sources
add_library(FN STATIC ${FN_SRCS} )
target_link_libraries ( FN FM gfortran) 

SET_TARGET_PROPERTIES( FN PROPERTIES LINKER_LANGUAGE CXX )
