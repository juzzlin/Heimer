# Set sources for the lib
set(LIB_SRC
    edge_context_menu.cpp
    main_context_menu.cpp
    recent_files_menu.cpp
    tool_bar.cpp
    widget_factory.cpp
)

# Add the library
add_library(${MENU_LIB_NAME} STATIC ${LIB_SRC})
target_link_libraries(${MENU_LIB_NAME} Qt${QT_VERSION_MAJOR}::Widgets SimpleLogger_static)

