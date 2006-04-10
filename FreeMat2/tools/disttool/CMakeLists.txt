set(DistTool_MOCS disttool.hpp)
set(DistTool_SRCS disttool.cpp)

# generate rules for building source files that moc generates
qt4_wrap_cpp(DistTool_MOC_SRCS ${DistTool_MOCS})

# build sources, moc'd sources, and rcc'd sources
add_executable(  DistTool ${DistTool_SRCS} ${DistTool_MOC_SRCS} )

target_link_libraries ( DistTool ${QT_LIBRARIES} ) 
