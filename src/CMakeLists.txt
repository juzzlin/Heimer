add_subdirectory(contrib/SimpleLogger EXCLUDE_FROM_ALL)
include_directories(contrib/SimpleLogger/src)

add_subdirectory(contrib/Argengine EXCLUDE_FROM_ALL)
include_directories(contrib/Argengine/src)

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
    application/application.cpp
    application/application_service.cpp
    application/control_strategy.cpp
    application/editor_service.cpp
    application/hash_seed.cpp
    application/progress_manager.cpp
    application/recent_files_manager.cpp
    application/service_container.cpp
    application/settings_proxy.cpp
    application/state_machine.cpp
    common/test_mode.cpp
    common/utils.cpp
    domain/copy_context.cpp
    domain/graph.cpp
    domain/image.cpp
    domain/image_manager.cpp
    domain/layout_optimizer.cpp
    domain/mind_map_data.cpp
    domain/mind_map_data_base.cpp
    domain/undo_stack.cpp
    infra/io/alz_file_io.cpp
    infra/io/alz_file_io_worker.cpp
    infra/io/xml_reader.cpp
    infra/io/xml_writer.cpp
    infra/settings.cpp
    infra/version_checker.cpp
    view/dialogs/about_dialog.cpp
    view/dialogs/color_dialog.cpp
    view/dialogs/color_setting_button.cpp
    view/dialogs/defaults_tab.cpp
    view/dialogs/editing_tab.cpp
    view/dialogs/effects_tab.cpp
    view/dialogs/export/png_export_dialog.cpp
    view/dialogs/export/svg_export_dialog.cpp
    view/dialogs/layout_optimization_dialog.cpp
    view/dialogs/scene_color_dialog.cpp
    view/dialogs/settings_dialog.cpp
    view/dialogs/settings_tab_base.cpp
    view/dialogs/spinner_dialog.cpp
    view/dialogs/whats_new_dialog.cpp
    view/dialogs/widget_factory.cpp
    view/edge_selection_group.cpp
    view/editor_scene.cpp
    view/editor_view.cpp
    view/grid.cpp
    view/item_filter.cpp
    view/magic_zoom.cpp
    view/main_window.cpp
    view/menus/edge_context_menu.cpp
    view/menus/main_context_menu.cpp
    view/menus/recent_files_menu.cpp
    view/menus/tool_bar.cpp
    view/menus/widget_factory.cpp
    view/mouse_action.cpp
    view/scene_items/edge.cpp
    view/scene_items/edge_dot.cpp
    view/scene_items/edge_text_edit.cpp
    view/scene_items/graphics_factory.cpp
    view/scene_items/node.cpp
    view/scene_items/node_handle.cpp
    view/scene_items/scene_item_base.cpp
    view/scene_items/text_edit.cpp
    view/node_selection_group.cpp
    view/widgets/font_button.cpp
    view/widgets/status_label.cpp
)

# Set sources for the lib (needed only for the IDE)
set(HEIMER_LIB_HDR
    application/application.hpp
    application/application_service.hpp
    application/control_strategy.hpp
    application/editor_service.hpp
    application/hash_seed.hpp
    application/progress_manager.hpp
    application/recent_files_manager.hpp
    application/service_container.hpp
    application/settings_proxy.hpp
    application/state_machine.hpp
    application/user_exception.hpp
    application/version.hpp
    common/constants.hpp
    common/test_mode.hpp
    common/types.hpp
    common/utils.hpp
    domain/copy_context.hpp
    domain/graph.hpp
    domain/image.hpp
    domain/image_manager.hpp
    domain/layout_optimizer.hpp
    domain/mind_map_data.hpp
    domain/mind_map_data_base.hpp
    domain/mind_map_stats.hpp
    domain/undo_stack.hpp
    infra/export_params.hpp
    infra/io/alz_file_io.hpp
    infra/io/alz_file_io_version.hpp
    infra/io/alz_file_io_worker.hpp
    infra/io/file_exception.hpp
    infra/io/file_io.hpp
    infra/io/xml_reader.hpp
    infra/io/xml_writer.hpp
    infra/settings.hpp
    infra/version_checker.hpp
    view/dialogs/about_dialog.hpp
    view/dialogs/color_dialog.hpp
    view/dialogs/color_setting_button.hpp
    view/dialogs/defaults_tab.hpp
    view/dialogs/editing_tab.hpp
    view/dialogs/effects_tab.hpp
    view/dialogs/export/png_export_dialog.hpp
    view/dialogs/export/svg_export_dialog.hpp
    view/dialogs/layout_optimization_dialog.hpp
    view/dialogs/scene_color_dialog.hpp
    view/dialogs/settings_dialog.hpp
    view/dialogs/settings_tab_base.hpp
    view/dialogs/spinner_dialog.hpp
    view/dialogs/whats_new_dialog.hpp
    view/dialogs/widget_factory.hpp
    view/edge_selection_group.hpp
    view/editor_scene.hpp
    view/editor_view.hpp
    view/grid.hpp
    view/item_filter.hpp
    view/magic_zoom.hpp
    view/main_window.hpp
    view/menus/edge_context_menu.hpp
    view/menus/main_context_menu.hpp
    view/menus/recent_files_menu.hpp
    view/menus/tool_bar.hpp
    view/menus/widget_factory.hpp
    view/mouse_action.hpp
    view/node_action.hpp
    view/node_selection_group.hpp
    view/scene_items/edge.hpp
    view/scene_items/edge_dot.hpp
    view/scene_items/edge_model.hpp
    view/scene_items/edge_point.hpp
    view/scene_items/edge_text_edit.hpp
    view/scene_items/graphics_factory.hpp
    view/scene_items/layers.hpp
    view/scene_items/node.hpp
    view/scene_items/node_handle.hpp
    view/scene_items/node_model.hpp
    view/scene_items/scene_item_base.hpp
    view/scene_items/text_edit.hpp
    view/shadow_effect_params.hpp
    view/widgets/font_button.hpp
    view/widgets/status_label.hpp
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
add_library(${HEIMER_LIB_NAME} STATIC ${HEIMER_LIB_HDR} ${HEIMER_LIB_SRC} ${MOC_SRC} ${RC_SRC} ${UI_HDRS} ${QM})
target_link_libraries(${HEIMER_LIB_NAME} Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Svg Qt${QT_VERSION_MAJOR}::Xml SimpleLogger_static Argengine_static)

# Add the executable
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
add_executable(${BINARY_NAME} WIN32 ${APP_SRC} ${MOC_SRC} ${RC_SRC} ${UI_HDRS} ${QM})
target_link_libraries(${BINARY_NAME} ${HEIMER_LIB_NAME})

# Add target for running clang-format
find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format)
if(NOT CLANG_FORMAT_EXECUTABLE)
    message(WARNING "clang-format not found. The 'style' target will not be available.")
else()
    add_custom_target(style COMMENT "Running clang-format on headers and sources")
    set(ALL_SOURCE_FILES ${HEIMER_LIB_SRC} ${HEIMER_LIB_HDR} ${APP_SRC})
    foreach(SOURCE_FILE ${ALL_SOURCE_FILES})
        add_custom_command(
            TARGET style
            COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${SOURCE_FILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Formatting ${SOURCE_FILE} with clang-format"
        )
    endforeach()
endif()
