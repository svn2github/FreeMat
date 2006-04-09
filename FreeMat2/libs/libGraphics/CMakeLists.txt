set(Graphics_SRCS  HandleAxis.cpp
HandleObject.cpp HandleProperty.cpp
HandleFigure.cpp HandleText.cpp
GLLabel.cpp GLRenderEngine.cpp
HandleLineSeries.cpp
RenderEngine.cpp HandleCommands.cpp
HandleImage.cpp HandleSurface.cpp
QTRenderEngine.cpp HandleWindow.cpp
)

# build sources, moc'd sources, and rcc'd sources
add_library(Graphics STATIC ${Graphics_SRCS})
target_link_libraries ( Graphics FM ${QT_LIBRARIES} ${QT_QTOPENGL_LIBRARY}) 

