set(XP_MOCS QTTerm.hpp KeyManager.hpp )
set(XP_SRCS DynLib.cpp System.cpp PathSearch.cpp KeyManager.cpp QTTerm.cpp)

# generate rules for building source files that moc generates
qt4_wrap_cpp(XP_MOC_SRCS ${XP_MOCS})

# build sources, moc'd sources, and rcc'd sources
add_library(XP STATIC ${XP_SRCS} ${XP_MOC_SRCS})
target_link_libraries ( XP FM ${QT_LIBRARIES} ) 

