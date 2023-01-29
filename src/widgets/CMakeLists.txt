# Set sources for the lib
set(LIB_SRC
    font_button.cpp
)

# Add the library
add_library(${WIDGET_LIB_NAME} STATIC ${LIB_SRC})
target_link_libraries(${WIDGET_LIB_NAME} Qt${QT_VERSION_MAJOR}::Widgets)

