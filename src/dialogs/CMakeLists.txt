# Set sources for the lib
set(LIB_SRC
    about_dialog.cpp
    color_dialog.cpp
    color_setting_button.cpp
    defaults_tab.cpp
    editing_tab.cpp
    effects_tab.cpp
    layout_optimization_dialog.cpp
    png_export_dialog.cpp
    scene_color_dialog.cpp
    settings_dialog.cpp
    svg_export_dialog.cpp
    whats_new_dialog.cpp
)

# Add the library
add_library(${DIALOGS_LIB_NAME} STATIC ${LIB_SRC})
target_link_libraries(${DIALOGS_LIB_NAME} Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Svg)

