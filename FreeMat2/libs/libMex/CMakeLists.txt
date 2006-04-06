set(Mex_SRCS  mxArray.cpp MexInterface.cpp
)

# build sources, moc'd sources, and rcc'd sources
add_library(Mex STATIC ${Mex_SRCS})
target_link_libraries ( Mex FM ${QT_LIBRARIES} ) 
