set(FreeMat_MOCS application.h  SocketCB.hpp FuncMode.hpp  MainApp.hpp) 
set(FreeMat_RCCS FreeMat.qrc)      
set(FreeMat_SRCS MainApp.cpp SocketCB.cpp application.cpp main.cpp  FuncMode.cpp
)

IF(UNIX AND NOT CYGWIN AND NOT APPLE)
set ( FreeMat_SRCS ${FreeMat_SRCS} Terminal.cpp DumbTerminal.cpp )
set ( FreeMat_MOCS ${FreeMat_MOCS} DumbTerminal.hpp Terminal.hpp )
ENDIF(UNIX AND NOT CYGWIN AND NOT APPLE)

# generate rules for building source files from the resources
qt4_add_resources(FreeMat_RCC_SRCS ${FreeMat_RCCS})
# generate rules for building source files that moc generates
qt4_wrap_cpp(FreeMat_MOC_SRCS ${FreeMat_MOCS})

# build sources, moc'd sources, and rcc'd sources
add_executable(  FreeMat ${FreeMat_SRCS} ${FreeMat_MOC_SRCS} ${FreeMat_RCC_SRCS})

IF(WIN32)
set(WINSOCK "-lws2_32")
ELSE(WIN32)
set(WINSOCK "")
ENDIF(WIN32)

target_link_libraries ( FreeMat XP Mex Graphics FN Core FM Mex ${QT_LIBRARIES} 
${OPENGL_LIBRARIES} ${AVCALL_LIBRARY} ${FFTW_LIBRARY} ${ARPACK_LIBRARY}
${FFTWF_LIBRARY}  ${MATIO_LIBRARY} ${UMFPACK_LIBRARY} ${AMD_LIBRARY}
${LAPACK_LIBRARY} ${BLAS_LIBRARY} ${FORT_LIBRARY} ${WINSOCK}) 

IF(NOT WIN32)
  target_link_libraries( FreeMat  ${CURSES_LIBRARY} )
ENDIF(NOT WIN32)


