set(Core_MOCS helpwidget.hpp )
set(Core_SRCS  Cast.cpp Constructors.cpp FFT.cpp
StringOps.cpp Transcendental.cpp
Trigonometric.cpp Misc.cpp
Constants.cpp Directory.cpp
Analyze.cpp Utils.cpp IO.cpp
Inspect.cpp Random.cpp
LoadCore.cpp MPIWrap.cpp
RanLib.cpp helpwidget.cpp
)


# generate rules for building source files that moc generates
qt4_wrap_cpp(Core_MOC_SRCS ${Core_MOCS})

# build sources, moc'd sources, and rcc'd sources
add_library(Core STATIC ${Core_SRCS} ${Core_MOC_SRCS})
target_link_libraries ( Core CrossLib FM ${QT_LIBRARIES} 
${AVCALL_LIBRARY} ${FFTW_LIBRARY} ${ARPACK_LIBRARY}
${FFTWF_LIBRARY}  ${UMFPACK_LIBRARY} ${AMD_LIBRARY}
${LAPACK_LIBRARY} ${BLAS_LIBRARY}) 

