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

find_program(GNUFORTRAN NAMES g77 gfortran)
message(STATUS "Fortran compiler ${GNUFORTRAN}")

IF(GNUFORTRAN MATCHES "g77")
  set(GNUFLIB "g2c")
ELSE(GNUFORTRAN MATCHES "g77")
  set(GNUFLIB "gfortran")
ENDIF(GNUFORTRAN MATCHES "g77")

add_custom_command(OUTPUT lmdif1.o COMMAND ${GNUFORTRAN} -c lmdif1.f)
add_custom_command(OUTPUT lmdif.o COMMAND ${GNUFORTRAN} -c lmdif.f)
add_custom_command(OUTPUT lmpar.o COMMAND ${GNUFORTRAN} -c lmpar.f)
add_custom_command(OUTPUT qrfac.o COMMAND ${GNUFORTRAN} -c qrfac.f)
add_custom_command(OUTPUT dpmpar.o COMMAND ${GNUFORTRAN} -c dpmpar.f)
add_custom_command(OUTPUT enorm.o COMMAND ${GNUFORTRAN} -c enorm.f)
add_custom_command(OUTPUT qrsolv.o COMMAND ${GNUFORTRAN} -c qrsolv.f)
add_custom_command(OUTPUT fdjac2.o COMMAND ${GNUFORTRAN} -c fdjac2.f)
add_custom_command(OUTPUT erf.o COMMAND ${GNUFORTRAN} -c erf.f)
add_custom_command(OUTPUT derf.o COMMAND ${GNUFORTRAN} -c derf.f)
add_custom_command(OUTPUT ei.o COMMAND ${GNUFORTRAN} -c ei.f)
add_custom_command(OUTPUT dei.o COMMAND ${GNUFORTRAN} -c dei.f)
add_custom_command(OUTPUT daw.o COMMAND ${GNUFORTRAN} -c daw.f)
add_custom_command(OUTPUT ddaw.o COMMAND ${GNUFORTRAN} -c ddaw.f)
add_custom_command(OUTPUT psi.o COMMAND ${GNUFORTRAN} -c psi.f)
add_custom_command(OUTPUT dpsi.o COMMAND ${GNUFORTRAN} -c dpsi.f)
add_custom_command(OUTPUT gamma.o COMMAND ${GNUFORTRAN} -c gamma.f)
add_custom_command(OUTPUT dgamma.o COMMAND ${GNUFORTRAN} -c dgamma.f)
add_custom_command(OUTPUT algama.o COMMAND ${GNUFORTRAN} -c algama.f)
add_custom_command(OUTPUT dlgama.o COMMAND ${GNUFORTRAN} -c dlgama.f)

# build sources, moc'd sources, and rcc'd sources
add_library(FN STATIC ${FN_SRCS} )
target_link_libraries ( FN FM ${GNUFLIB}) 

SET_TARGET_PROPERTIES( FN PROPERTIES LINKER_LANGUAGE CXX )
