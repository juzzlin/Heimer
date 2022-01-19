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
set(LIB_SRC
    about_dlg.cpp
    alz_serializer.cpp
    application.cpp
    color_dialog.cpp
    constants.hpp
    copy_context.cpp
    defaults_tab.cpp
    edge.cpp
    edge_context_menu.cpp
    edge_dot.cpp
    edge_point.hpp
    edge_text_edit.cpp
    editing_tab.cpp
    editor_data.cpp
    editor_scene.cpp
    editor_view.cpp
    effects_tab.cpp
    file_exception.hpp
    graph.cpp
    graphics_factory.cpp
    grid.cpp
    hash_seed.cpp
    image.cpp
    image_manager.cpp
    item_filter.cpp
    layers.hpp
    layout_optimization_dialog.cpp
    layout_optimizer.cpp
    magic_zoom.cpp
    main_context_menu.cpp
    main_window.cpp
    mediator.cpp
    mind_map_data.cpp
    mind_map_data_base.cpp
    mouse_action.cpp
    node.cpp
    node_action.hpp
    node_handle.cpp
    png_export_dialog.cpp
    recent_files_manager.cpp
    recent_files_menu.cpp
    selection_group.cpp
    settings.cpp
    settings_dialog.cpp
    settings_proxy.cpp
    state_machine.cpp
    svg_export_dialog.cpp
    test_mode.cpp
    text_edit.cpp
    tool_bar.cpp
    types.hpp
    undo_stack.cpp
    user_exception.hpp
    version.hpp
    version_checker.cpp
    whats_new_dlg.cpp
    widget_factory.cpp
    xml_reader.cpp
    xml_writer.cpp
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
add_library(${LIBRARY_NAME} STATIC ${LIB_SRC} ${MOC_SRC} ${RC_SRC} ${UI_HDRS} ${QM})
target_link_libraries(${LIBRARY_NAME} Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Svg Qt${QT_VERSION_MAJOR}::Xml Qt${QT_VERSION_MAJOR}::Network SimpleLogger_static Argengine_static)

# Add the executable
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
add_executable(${BINARY_NAME} WIN32 ${APP_SRC} ${MOC_SRC} ${RC_SRC} ${UI_HDRS} ${QM})
target_link_libraries(${BINARY_NAME} Qt${QT_VERSION_MAJOR}::Widgets ${LIBRARY_NAME})
