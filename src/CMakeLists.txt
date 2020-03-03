add_subdirectory(contrib/SimpleLogger EXCLUDE_FROM_ALL)
include_directories(contrib/SimpleLogger/src)

add_subdirectory(contrib/Argengine EXCLUDE_FROM_ALL)
include_directories(contrib/Argengine/src)

# Translation files in src/translations (without .ts)
set(TS heimer_fi heimer_fr heimer_it)
set(TS_FILES)
set(QM_FILES)
foreach(TS_FILE ${TS})
    list(APPEND TS_FILES ${CMAKE_SOURCE_DIR}/src/translations/${TS_FILE}.ts)
    list(APPEND QM_FILES ${CMAKE_BINARY_DIR}/data/translations/${TS_FILE}.qm)
endforeach()
set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${CMAKE_BINARY_DIR}/data/translations)

# Set sources
set(SRC
    about_dlg.cpp
    application.cpp
    constants.hpp
    copy_paste.cpp
    edge.cpp
    edge_base.cpp
    edge_context_menu.cpp
    edge_dot.cpp
    edge_point.hpp
    edge_text_edit.cpp
    file_exception.hpp
    graph.cpp
    graphics_factory.cpp
    grid.cpp
    hash_seed.cpp
    editor_data.cpp
    editor_scene.cpp
    editor_view.cpp
    image.cpp
    image_manager.cpp
    png_export_dialog.cpp
    main.cpp
    main_context_menu.cpp
    main_window.cpp
    magic_zoom.cpp
    mediator.cpp
    mind_map_data.cpp
    mind_map_data_base.cpp
    mouse_action.cpp
    node.cpp
    node_base.cpp
    node_handle.cpp
    reader.cpp
    recent_files_manager.cpp
    recent_files_menu.cpp
    selection_group.cpp
    serializer.cpp
    state_machine.cpp
    text_edit.cpp
    undo_stack.cpp
    user_exception.hpp
    layers.hpp
    whats_new_dlg.cpp
    writer.cpp
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

qt5_add_resources(RC_SRC ${RCS})
qt5_add_translation(QM ${TS_FILES})
qt5_wrap_ui(UI_HDRS ${UIS})

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
    set(SRC ${SRC} ${CMAKE_CURRENT_BINARY_DIR}/windowsrc.o)
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

# Add the executable
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
add_executable(${BINARY_NAME} WIN32 ${SRC} ${MOC_SRC} ${RC_SRC} ${UI_HDRS} ${QM})

target_link_libraries(${BINARY_NAME} Qt5::Widgets Qt5::Xml SimpleLogger_static Argengine_static)
