set(BlasTune_SRCS blastune.cpp)

# build sources, moc'd sources, and rcc'd sources
add_executable(  BlasTune ${BlasTune_SRCS}  )

target_link_libraries ( BlasTune ${QT_LIBRARIES} ) 
