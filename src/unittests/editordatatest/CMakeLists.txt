set(EDITOR_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories(${EDITOR_DIR} ${EDITOR_DIR}/contrib ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DHEIMER_UNIT_TEST)

set(NAME editordatatest)
set(SRC ${NAME}.cpp
    ${EDITOR_DIR}/draganddropstore.cpp
    ${EDITOR_DIR}/edge.cpp
    ${EDITOR_DIR}/edgebase.cpp
    ${EDITOR_DIR}/edgedot.cpp
    ${EDITOR_DIR}/edgetextedit.cpp
    ${EDITOR_DIR}/editordata.cpp
    ${EDITOR_DIR}/graph.cpp
    ${EDITOR_DIR}/graphicsfactory.cpp
    ${EDITOR_DIR}/hashseed.cpp
    ${EDITOR_DIR}/mindmapdata.cpp
    ${EDITOR_DIR}/mindmapdatabase.cpp
    ${EDITOR_DIR}/node.cpp
    ${EDITOR_DIR}/nodebase.cpp
    ${EDITOR_DIR}/nodehandle.cpp
    ${EDITOR_DIR}/reader.cpp
    ${EDITOR_DIR}/serializer.cpp
    ${EDITOR_DIR}/textedit.cpp
    ${EDITOR_DIR}/undostack.cpp
    ${EDITOR_DIR}/writer.cpp
    ${EDITOR_DIR}/contrib/mclogger.cc
    mediator_mock.cpp
    )

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(${NAME} ${SRC} ${MOC_SRC})
add_test(${NAME} ${CMAKE_SOURCE_DIR}/unittests/${NAME})

qt5_use_modules(${NAME} Test Xml Widgets)

set_property(TARGET ${NAME} PROPERTY CXX_STANDARD 11)
