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
  find_library(GNUFLIB NAMES g2c PATHS /usr/lib /usr/local/lib /sw/lib)
ELSE(GNUFORTRAN MATCHES "g77")
  find_library(GNUFLIB NAMES gfortran PATHS /usr/lib /usr/local/lib /sw/lib)
ENDIF(GNUFORTRAN MATCHES "g77")

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lmdif1.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/lmdif1.f -o ${CMAKE_CURRENT_BINARY_DIR}/lmdif1.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lmdif.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/lmdif.f -o ${CMAKE_CURRENT_BINARY_DIR}/lmdif.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lmpar.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/lmpar.f -o ${CMAKE_CURRENT_BINARY_DIR}/lmpar.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/qrfac.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/qrfac.f -o ${CMAKE_CURRENT_BINARY_DIR}/qrfac.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dpmpar.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/dpmpar.f -o ${CMAKE_CURRENT_BINARY_DIR}/dpmpar.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/enorm.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/enorm.f -o ${CMAKE_CURRENT_BINARY_DIR}/enorm.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/qrsolv.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/qrsolv.f -o ${CMAKE_CURRENT_BINARY_DIR}/qrsolv.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/fdjac2.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/fdjac2.f -o ${CMAKE_CURRENT_BINARY_DIR}/fdjac2.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/erf.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/erf.f -o ${CMAKE_CURRENT_BINARY_DIR}/erf.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/derf.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/derf.f -o ${CMAKE_CURRENT_BINARY_DIR}/derf.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ei.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/ei.f -o ${CMAKE_CURRENT_BINARY_DIR}/ei.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dei.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/dei.f -o ${CMAKE_CURRENT_BINARY_DIR}/dei.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/daw.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/daw.f  -o ${CMAKE_CURRENT_BINARY_DIR}/daw.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ddaw.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/ddaw.f -o ${CMAKE_CURRENT_BINARY_DIR}/ddaw.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/psi.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/psi.f -o ${CMAKE_CURRENT_BINARY_DIR}/psi.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dpsi.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/dpsi.f -o ${CMAKE_CURRENT_BINARY_DIR}/dpsi.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gamma.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/gamma.f -o ${CMAKE_CURRENT_BINARY_DIR}/gamma.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dgamma.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/dgamma.f -o ${CMAKE_CURRENT_BINARY_DIR}/dgamma.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/algama.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/algama.f -o ${CMAKE_CURRENT_BINARY_DIR}/algama.o)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dlgama.o COMMAND ${GNUFORTRAN} -c ${CMAKE_CURRENT_SOURCE_DIR}/dlgama.f -o ${CMAKE_CURRENT_BINARY_DIR}/dlgama.o)

# build sources, moc'd sources, and rcc'd sources
add_library(FN STATIC ${FN_SRCS} )
target_link_libraries ( FN FM ${GNUFLIB}) 

SET_TARGET_PROPERTIES( FN PROPERTIES LINKER_LANGUAGE CXX )
