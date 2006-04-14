set(BlasTune_SRCS blastune.cpp)

# build sources, moc'd sources, and rcc'd sources
add_executable(  BlasTune ${BlasTune_SRCS}  )

target_link_libraries ( BlasTune ${QT_LIBRARIES} ) 

set(BlasTuneApp_SRCS blastuneapp.cpp)
set(BlasTuneApp_MOCS blastuneapp.hpp)
qt4_wrap_cpp(BlasTuneApp_MOC_SRCS ${BlasTuneApp_MOCS})
add_executable(  BlasTuneApp ${BlasTuneApp_SRCS} ${BlasTuneApp_MOC_SRCS} )
target_link_libraries ( BlasTuneApp ${QT_LIBRARIES} ) 
