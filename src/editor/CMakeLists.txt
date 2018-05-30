# Translation files in src/editor/translations (without .ts)
#set(TS heimer_fi)
set(TS_FILES)
set(QM_FILES)
foreach(TS_FILE ${TS})
    list(APPEND TS_FILES ${CMAKE_SOURCE_DIR}/src/editor/translations/${TS_FILE}.ts)
    list(APPEND QM_FILES ${CMAKE_BINARY_DIR}/src/editor/${TS_FILE}.qm)
endforeach()

# Set sources
set(SRC
    aboutdlg.cpp
    application.cpp
    config.hpp
    draganddropstore.cpp
    edge.cpp
    edgebase.cpp
    edgedot.cpp
    edgetextedit.cpp
    fileexception.hpp
    graph.cpp
    graphicsfactory.cpp
    hashseed.cpp
    editordata.cpp
    editorscene.cpp
    editorview.cpp
    main.cpp
    mainwindow.cpp
    mediator.cpp
    mindmapdata.cpp
    mindmapdatabase.cpp
    node.cpp
    nodebase.cpp
    nodehandle.cpp
    reader.cpp
    serializer.cpp
    textedit.cpp
    undostack.cpp
    userexception.hpp
    contrib/mclogger.cc
    layers.hpp
    writer.cpp
)

set(RCS ${CMAKE_SOURCE_DIR}/data/images/images.qrc ${CMAKE_SOURCE_DIR}/data/icons/icons.qrc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/contrib)

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

target_link_libraries(${BINARY_NAME} Qt5::Widgets Qt5::Xml)
set_property(TARGET ${BINARY_NAME} PROPERTY CXX_STANDARD 11)

foreach(TS_FILE ${TS})
    # Make targets to copy generated qm files to data dir. This is done the hard
    # way, because qt5_add_translation() generates the qm files to ${CMAKE_CURRENT_SOURCE_DIR}
    add_custom_target(copy-translation-file-${TS_FILE} ALL
        COMMAND cmake -E copy ${CMAKE_BINARY_DIR}/src/editor/${TS_FILE}.qm
            ${CMAKE_BINARY_DIR}/data/translations/${TS_FILE}.qm
        DEPENDS ${BINARY_NAME})
endforeach()
