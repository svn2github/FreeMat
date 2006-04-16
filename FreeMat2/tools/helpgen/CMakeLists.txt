set(FMHelpGen_MOCS helpgen.hpp) 
set(FMHelpGen_SRCS helpgen.cpp)

# generate rules for building source files that moc generates
qt4_wrap_cpp(FMHelpGen_MOC_SRCS ${FMHelpGen_MOCS})

# build sources, moc'd sources, and rcc'd sources
add_executable(  FMHelpGen ${FMHelpGen_SRCS} ${FMHelpGen_MOC_SRCS})

IF(WIN32)
set(WINSOCK "-lws2_32")
ELSE(WIN32)
set(WINSOCK "")
ENDIF(WIN32)

target_link_libraries ( FMHelpGen CrossLib Mex Graphics FN Core FM Mex ${QT_LIBRARIES} 
${OPENGL_LIBRARIES} ${AVCALL_LIBRARY} ${FFTW_LIBRARY} ${ARPACK_LIBRARY}
${FFTWF_LIBRARY}  ${MATIO_LIBRARY} ${UMFPACK_LIBRARY} ${AMD_LIBRARY}
${LAPACK_LIBRARY} ${BLAS_LIBRARY} ${FORT_LIBRARY} ${WINSOCK}
${ZLIB_LIBRARY} ${FORTRAN_TO_C_LIBRARY})


