set(FreeMat_MOCS application.h PathTool.hpp ToolDock.hpp helpgen.hpp
DumbTerminal.hpp SocketCB.hpp highlighter.hpp
FuncMode.hpp HistoryWidget.hpp VariablesTool.hpp
Terminal.hpp FileTool.hpp MainApp.hpp Editor.hpp) 
set(FreeMat_RCCS ../FreeMat.qrc)
set(FreeMat_SRCS MainApp.cpp
SocketCB.cpp application.cpp main.cpp highlighter.cpp
helpgen.cpp PathTool.cpp Editor.cpp ToolDock.cpp
HistoryWidget.cpp FileTool.cpp Common.cpp FuncMode.cpp
VariablesTool.cpp Terminal.cpp DumbTerminal.cpp
)

# generate rules for building source files from the resources
qt4_add_resources(FreeMat_RCC_SRCS ${FreeMat_RCCS})
# generate rules for building source files that moc generates
qt4_wrap_cpp(FreeMat_MOC_SRCS ${FreeMat_MOCS})

# build sources, moc'd sources, and rcc'd sources
add_executable(  FreeMat ${FreeMat_SRCS} ${FreeMat_MOC_SRCS} ${FreeMat_RCC_SRCS})

target_link_libraries ( FreeMat XP Mex Graphics FN Core FM ${QT_LIBRARIES} ${QT_QTOPENGL_LIBRARY}
${AVCALL_LIBRARY} ${FFTW_LIBRARY} ${ARPACK_LIBRARY}
${FFTWF_LIBRARY}  ${MATIO_LIBRARY} ${UMFPACK_LIBRARY} ${AMD_LIBRARY}
${LAPACK_LIBRARY} ${BLAS_LIBRARY} ${FORT_LIBRARY} ${CURSES_LIBRARY}) 

