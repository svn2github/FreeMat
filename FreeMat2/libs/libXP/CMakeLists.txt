set(CrossLib_MOCS QTTerm.hpp KeyManager.hpp PathTool.hpp ToolDock.hpp
highlighter.hpp HistoryWidget.hpp VariablesTool.hpp FileTool.hpp  
Editor.hpp)
set(CrossLib_SRCS DynLib.cpp System.cpp PathSearch.cpp KeyManager.cpp
QTTerm.cpp  highlighter.cpp  PathTool.cpp Editor.cpp ToolDock.cpp
HistoryWidget.cpp FileTool.cpp VariablesTool.cpp Common.cpp)

# generate rules for building source files that moc generates
qt4_wrap_cpp(CrossLib_MOC_SRCS ${CrossLib_MOCS})

# build sources, moc'd sources, and rcc'd sources
add_library(CrossLib STATIC ${CrossLib_SRCS} ${CrossLib_MOC_SRCS})
target_link_libraries ( CrossLib FM ${QT_LIBRARIES} ) 

