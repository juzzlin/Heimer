add_subdirectory(contrib/SimpleLogger EXCLUDE_FROM_ALL)
include_directories(contrib/SimpleLogger/src)

add_subdirectory(contrib/Argengine EXCLUDE_FROM_ALL)
include_directories(contrib/Argengine/src)

# This lib can be removed once we can upgrade to C++20
add_subdirectory(contrib/range-v3-0.11.0 EXCLUDE_FROM_ALL)
include_directories(contrib/range-v3-0.11.0/include)

# Translation files in src/translations (without .ts)
set(TS heimer_de heimer_en heimer_es heimer_fi heimer_fr heimer_it heimer_nl heimer_zh)
set(TS_FILES)
set(QM_FILES)
foreach(TS_FILE ${TS})
    list(APPEND TS_FILES ${CMAKE_SOURCE_DIR}/src/translations/${TS_FILE}.ts)
    list(APPEND QM_FILES ${CMAKE_BINARY_DIR}/data/translations/${TS_FILE}.qm)
endforeach()
set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${CMAKE_BINARY_DIR}/data/translations)

# Set sources for the lib
set(HEIMER_LIB_SRC
    application.cpp
    constants.hpp
    control_strategy.cpp
    copy_context.cpp
    editor_data.cpp
    editor_scene.cpp
    editor_view.cpp
    grid.cpp
    image.cpp
    image_manager.cpp
    item_filter.cpp
    layout_optimizer.cpp
    magic_zoom.cpp
    main_window.cpp
    mediator.cpp
    mouse_action.cpp
    node_action.hpp
    recent_files_manager.cpp
    selection_group.cpp
    state_machine.cpp
    types.hpp
    utils.cpp

    core/graph.cpp
    core/hash_seed.cpp
    core/mind_map_data.cpp
    core/mind_map_data_base.cpp
    core/progress_manager.cpp
    core/settings.cpp
    core/settings_proxy.cpp
    core/shadow_effect_params.hpp
    core/single_instance_container.cpp
    core/test_mode.cpp
    core/undo_stack.cpp
    core/user_exception.hpp
    core/version.hpp
    core/version_checker.cpp

    dialogs/about_dialog.cpp
    dialogs/color_dialog.cpp
    dialogs/color_setting_button.cpp
    dialogs/defaults_tab.cpp
    dialogs/editing_tab.cpp
    dialogs/effects_tab.cpp
    dialogs/layout_optimization_dialog.cpp
    dialogs/png_export_dialog.cpp
    dialogs/scene_color_dialog.cpp
    dialogs/settings_dialog.cpp
    dialogs/settings_tab_base.cpp
    dialogs/spinner_dialog.cpp
    dialogs/svg_export_dialog.cpp
    dialogs/whats_new_dialog.cpp
    dialogs/widget_factory.cpp

    io/alz_file_io.cpp
    io/alz_file_io_worker.cpp
    io/file_exception.hpp
    io/file_io.hpp
    io/xml_reader.cpp
    io/xml_writer.cpp

    menus/edge_context_menu.cpp
    menus/main_context_menu.cpp
    menus/recent_files_menu.cpp
    menus/tool_bar.cpp
    menus/widget_factory.cpp

    scene_items/edge.cpp
    scene_items/edge_dot.cpp
    scene_items/edge_model.hpp
    scene_items/edge_point.hpp
    scene_items/edge_text_edit.cpp
    scene_items/graphics_factory.cpp
    scene_items/layers.hpp
    scene_items/node.cpp
    scene_items/node_handle.cpp
    scene_items/node_model.hpp
    scene_items/scene_item_base.cpp
    scene_items/text_edit.cpp

    widgets/font_button.cpp
    widgets/status_label.cpp
)

# Set sources for the app
set(APP_SRC
    main.cpp
)

# Need to copy the QRC file to binary dir where the QM-files will be generated.
# This is because the paths in translations.qrc are relative to itself.
configure_file(${CMAKE_SOURCE_DIR}/data/translations/translations.qrc ${CMAKE_BINARY_DIR}/data/translations/translations.qrc COPYONLY)

set(RCS
    ${CMAKE_SOURCE_DIR}/meta.qrc
    ${CMAKE_SOURCE_DIR}/data/images/images.qrc
    ${CMAKE_SOURCE_DIR}/data/icons/icons.qrc
    ${CMAKE_BINARY_DIR}/data/translations/translations.qrc
)

if(${QT_VERSION_MAJOR} STREQUAL "6")
    qt_add_resources(RC_SRC ${RCS})
    qt_add_translation(QM ${TS_FILES})
    qt_wrap_ui(UI_HDRS ${UIS})
# This is needed for Qt < 5.15
else()
    qt5_add_resources(RC_SRC ${RCS})
    qt5_add_translation(QM ${TS_FILES})
    qt5_wrap_ui(UI_HDRS ${UIS})
endif()

# We need this to be able to include headers produced by uic in our code
include_directories(${CMAKE_BINARY_DIR})

# Resource compilation for MinGW
if(MINGW)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o
        COMMAND
            ${CMAKE_RC_COMPILER}
            -I${CMAKE_SOURCE_DIR} -i${CMAKE_SOURCE_DIR}/data/icons/WindowsHeimer.rc
            -o ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o)
    set(APP_SRC ${APP_SRC} ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o)
endif()

# Copy desktop file
if(UNIX)
    set(DesktopFileSourcePath)
    if(USC)
        set(DesktopFileSourcePath ${CMAKE_SOURCE_DIR}/src/heimer.desktop.opt.in)
    else()
        set(DesktopFileSourcePath ${CMAKE_SOURCE_DIR}/src/heimer.desktop.in)
    endif()
    add_custom_target(desktop-file ALL
        COMMAND cmake -E copy ${DesktopFileSourcePath}
            ${CMAKE_BINARY_DIR}/heimer.desktop
    DEPENDS ${BINARY_NAME})
endif()

# Add the library
add_library(${HEIMER_LIB_NAME} STATIC ${HEIMER_LIB_SRC} ${MOC_SRC} ${RC_SRC} ${UI_HDRS} ${QM})
target_link_libraries(${HEIMER_LIB_NAME} Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Svg Qt${QT_VERSION_MAJOR}::Xml SimpleLogger_static Argengine_static)

# Add the executable
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
add_executable(${BINARY_NAME} WIN32 ${APP_SRC} ${MOC_SRC} ${RC_SRC} ${UI_HDRS} ${QM})
target_link_libraries(${BINARY_NAME} ${HEIMER_LIB_NAME})
