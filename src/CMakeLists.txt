add_subdirectory(contrib/SimpleLogger EXCLUDE_FROM_ALL)
include_directories(contrib/SimpleLogger/src)

add_subdirectory(contrib/Argengine EXCLUDE_FROM_ALL)
include_directories(contrib/Argengine/src)

set(HEIMER_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Translation files in src/translations (without .ts)
set(TS heimer_de heimer_en heimer_es heimer_eu heimer_fi heimer_fr heimer_it heimer_nl heimer_pt_Br heimer_pt_Pt heimer_zh)
set(TS_FILES)
set(QM_FILES)
foreach(TS_FILE ${TS})
    list(APPEND TS_FILES ${HEIMER_SRC_ROOT}/translations/${TS_FILE}.ts)
    list(APPEND QM_FILES ${CMAKE_BINARY_DIR}/data/translations/${TS_FILE}.qm)
endforeach()
set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${CMAKE_BINARY_DIR}/data/translations)

# Set sources for the lib
set(HEIMER_LIB_SRC
    ${HEIMER_SRC_ROOT}/application/application.cpp
    ${HEIMER_SRC_ROOT}/application/application_service.cpp
    ${HEIMER_SRC_ROOT}/application/control_strategy.cpp
    ${HEIMER_SRC_ROOT}/application/editor_service.cpp
    ${HEIMER_SRC_ROOT}/application/hash_seed.cpp
    ${HEIMER_SRC_ROOT}/application/language_service.cpp
    ${HEIMER_SRC_ROOT}/application/progress_manager.cpp
    ${HEIMER_SRC_ROOT}/application/recent_files_manager.cpp
    ${HEIMER_SRC_ROOT}/application/service_container.cpp
    ${HEIMER_SRC_ROOT}/application/settings_proxy.cpp
    ${HEIMER_SRC_ROOT}/application/state_machine.cpp
    ${HEIMER_SRC_ROOT}/common/constants.cpp
    ${HEIMER_SRC_ROOT}/common/test_mode.cpp
    ${HEIMER_SRC_ROOT}/common/utils.cpp
    ${HEIMER_SRC_ROOT}/domain/copy_context.cpp
    ${HEIMER_SRC_ROOT}/domain/graph.cpp
    ${HEIMER_SRC_ROOT}/domain/image.cpp
    ${HEIMER_SRC_ROOT}/domain/image_manager.cpp
    ${HEIMER_SRC_ROOT}/domain/layout_optimizer.cpp
    ${HEIMER_SRC_ROOT}/domain/mind_map_data.cpp
    ${HEIMER_SRC_ROOT}/domain/mind_map_data_base.cpp
    ${HEIMER_SRC_ROOT}/domain/undo_stack.cpp
    ${HEIMER_SRC_ROOT}/infra/io/alz_file_io.cpp
    ${HEIMER_SRC_ROOT}/infra/io/alz_file_io_worker.cpp
    ${HEIMER_SRC_ROOT}/infra/io/xml_reader.cpp
    ${HEIMER_SRC_ROOT}/infra/io/xml_writer.cpp
    ${HEIMER_SRC_ROOT}/infra/settings.cpp
    ${HEIMER_SRC_ROOT}/infra/version_checker.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/about_dialog.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/color_dialog.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/color_setting_button.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/defaults_tab.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/editing_tab.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/effects_tab.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/export/png_export_dialog.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/export/svg_export_dialog.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/layout_optimization_dialog.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/scene_color_dialog.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/settings_dialog.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/settings_tab_base.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/spinner_dialog.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/whats_new_dialog.cpp
    ${HEIMER_SRC_ROOT}/view/dialogs/widget_factory.cpp
    ${HEIMER_SRC_ROOT}/view/edge_selection_group.cpp
    ${HEIMER_SRC_ROOT}/view/editor_scene.cpp
    ${HEIMER_SRC_ROOT}/view/editor_view.cpp
    ${HEIMER_SRC_ROOT}/view/grid.cpp
    ${HEIMER_SRC_ROOT}/view/item_filter.cpp
    ${HEIMER_SRC_ROOT}/view/magic_zoom.cpp
    ${HEIMER_SRC_ROOT}/view/main_window.cpp
    ${HEIMER_SRC_ROOT}/view/menus/edge_context_menu.cpp
    ${HEIMER_SRC_ROOT}/view/menus/main_context_menu.cpp
    ${HEIMER_SRC_ROOT}/view/menus/main_menu.cpp
    ${HEIMER_SRC_ROOT}/view/menus/recent_files_menu.cpp
    ${HEIMER_SRC_ROOT}/view/menus/tool_bar.cpp
    ${HEIMER_SRC_ROOT}/view/menus/widget_factory.cpp
    ${HEIMER_SRC_ROOT}/view/mouse_action.cpp
    ${HEIMER_SRC_ROOT}/view/node_selection_group.cpp
    ${HEIMER_SRC_ROOT}/view/scene_items/edge.cpp
    ${HEIMER_SRC_ROOT}/view/scene_items/edge_dot.cpp
    ${HEIMER_SRC_ROOT}/view/scene_items/edge_text_edit.cpp
    ${HEIMER_SRC_ROOT}/view/scene_items/graphics_factory.cpp
    ${HEIMER_SRC_ROOT}/view/scene_items/node.cpp
    ${HEIMER_SRC_ROOT}/view/scene_items/node_handle.cpp
    ${HEIMER_SRC_ROOT}/view/scene_items/scene_item_base.cpp
    ${HEIMER_SRC_ROOT}/view/scene_items/text_edit.cpp
    ${HEIMER_SRC_ROOT}/view/widgets/font_button.cpp
    ${HEIMER_SRC_ROOT}/view/widgets/status_label.cpp
)

# Set sources for the lib (needed only for the IDE)
set(HEIMER_LIB_HDR
    ${HEIMER_SRC_ROOT}/application/application.hpp
    ${HEIMER_SRC_ROOT}/application/application_service.hpp
    ${HEIMER_SRC_ROOT}/application/control_strategy.hpp
    ${HEIMER_SRC_ROOT}/application/editor_service.hpp
    ${HEIMER_SRC_ROOT}/application/hash_seed.hpp
    ${HEIMER_SRC_ROOT}/application/language_service.hpp
    ${HEIMER_SRC_ROOT}/application/progress_manager.hpp
    ${HEIMER_SRC_ROOT}/application/recent_files_manager.hpp
    ${HEIMER_SRC_ROOT}/application/service_container.hpp
    ${HEIMER_SRC_ROOT}/application/settings_proxy.hpp
    ${HEIMER_SRC_ROOT}/application/state_machine.hpp
    ${HEIMER_SRC_ROOT}/application/user_exception.hpp
    ${HEIMER_SRC_ROOT}/application/version.hpp
    ${HEIMER_SRC_ROOT}/common/constants.hpp
    ${HEIMER_SRC_ROOT}/common/test_mode.hpp
    ${HEIMER_SRC_ROOT}/common/types.hpp
    ${HEIMER_SRC_ROOT}/common/utils.hpp
    ${HEIMER_SRC_ROOT}/domain/copy_context.hpp
    ${HEIMER_SRC_ROOT}/domain/graph.hpp
    ${HEIMER_SRC_ROOT}/domain/image.hpp
    ${HEIMER_SRC_ROOT}/domain/image_manager.hpp
    ${HEIMER_SRC_ROOT}/domain/layout_optimizer.hpp
    ${HEIMER_SRC_ROOT}/domain/mind_map_data.hpp
    ${HEIMER_SRC_ROOT}/domain/mind_map_data_base.hpp
    ${HEIMER_SRC_ROOT}/domain/mind_map_stats.hpp
    ${HEIMER_SRC_ROOT}/domain/undo_stack.hpp
    ${HEIMER_SRC_ROOT}/infra/export_params.hpp
    ${HEIMER_SRC_ROOT}/infra/io/alz_file_io.hpp
    ${HEIMER_SRC_ROOT}/infra/io/alz_file_io_version.hpp
    ${HEIMER_SRC_ROOT}/infra/io/alz_file_io_worker.hpp
    ${HEIMER_SRC_ROOT}/infra/io/file_exception.hpp
    ${HEIMER_SRC_ROOT}/infra/io/file_io.hpp
    ${HEIMER_SRC_ROOT}/infra/io/xml_reader.hpp
    ${HEIMER_SRC_ROOT}/infra/io/xml_writer.hpp
    ${HEIMER_SRC_ROOT}/infra/settings.hpp
    ${HEIMER_SRC_ROOT}/infra/version_checker.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/about_dialog.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/color_dialog.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/color_setting_button.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/defaults_tab.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/editing_tab.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/effects_tab.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/export/png_export_dialog.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/export/svg_export_dialog.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/layout_optimization_dialog.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/scene_color_dialog.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/settings_dialog.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/settings_tab_base.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/spinner_dialog.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/whats_new_dialog.hpp
    ${HEIMER_SRC_ROOT}/view/dialogs/widget_factory.hpp
    ${HEIMER_SRC_ROOT}/view/edge_action.hpp
    ${HEIMER_SRC_ROOT}/view/edge_selection_group.hpp
    ${HEIMER_SRC_ROOT}/view/editor_scene.hpp
    ${HEIMER_SRC_ROOT}/view/editor_view.hpp
    ${HEIMER_SRC_ROOT}/view/grid.hpp
    ${HEIMER_SRC_ROOT}/view/item_filter.hpp
    ${HEIMER_SRC_ROOT}/view/magic_zoom.hpp
    ${HEIMER_SRC_ROOT}/view/main_window.hpp
    ${HEIMER_SRC_ROOT}/view/menus/edge_context_menu.hpp
    ${HEIMER_SRC_ROOT}/view/menus/main_context_menu.hpp
    ${HEIMER_SRC_ROOT}/view/menus/main_menu.hpp
    ${HEIMER_SRC_ROOT}/view/menus/recent_files_menu.hpp
    ${HEIMER_SRC_ROOT}/view/menus/tool_bar.hpp
    ${HEIMER_SRC_ROOT}/view/menus/widget_factory.hpp
    ${HEIMER_SRC_ROOT}/view/mouse_action.hpp
    ${HEIMER_SRC_ROOT}/view/node_action.hpp
    ${HEIMER_SRC_ROOT}/view/node_selection_group.hpp
    ${HEIMER_SRC_ROOT}/view/scene_items/edge.hpp
    ${HEIMER_SRC_ROOT}/view/scene_items/edge_dot.hpp
    ${HEIMER_SRC_ROOT}/view/scene_items/edge_model.hpp
    ${HEIMER_SRC_ROOT}/view/scene_items/edge_point.hpp
    ${HEIMER_SRC_ROOT}/view/scene_items/edge_text_edit.hpp
    ${HEIMER_SRC_ROOT}/view/scene_items/graphics_factory.hpp
    ${HEIMER_SRC_ROOT}/view/scene_items/layers.hpp
    ${HEIMER_SRC_ROOT}/view/scene_items/node.hpp
    ${HEIMER_SRC_ROOT}/view/scene_items/node_handle.hpp
    ${HEIMER_SRC_ROOT}/view/scene_items/node_model.hpp
    ${HEIMER_SRC_ROOT}/view/scene_items/scene_item_base.hpp
    ${HEIMER_SRC_ROOT}/view/scene_items/text_edit.hpp
    ${HEIMER_SRC_ROOT}/view/shadow_effect_params.hpp
    ${HEIMER_SRC_ROOT}/view/widgets/font_button.hpp
    ${HEIMER_SRC_ROOT}/view/widgets/status_label.hpp
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
        set(DesktopFileSourcePath ${HEIMER_SRC_ROOT}/heimer.desktop.opt.in)
    else()
        set(DesktopFileSourcePath ${HEIMER_SRC_ROOT}/heimer.desktop.in)
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

# Specify the path to lupdate executable
set(LUPDATE_COMMAND "lupdate")
find_program(LUPDATE_EXECUTABLE ${LUPDATE_COMMAND})
if(NOT LUPDATE_EXECUTABLE)
    message(WARNING "${LUPDATE_COMMAND} not found. The 'update_translations' target will not be available.")
else()
    add_custom_target(update_translations)
    foreach(TS_FILE ${TS_FILES})
        get_filename_component(TS_FILE_NAME ${TS_FILE} NAME)
        add_custom_command(
            TARGET update_translations
            COMMAND ${LUPDATE_EXECUTABLE} -locations none -no-ui-lines -no-obsolete
            ARGS ${HEIMER_LIB_SRC} -ts ${TS_FILE}
            COMMENT "Updating translation source ${TS_FILE_NAME}"
        )
    endforeach()
endif()
