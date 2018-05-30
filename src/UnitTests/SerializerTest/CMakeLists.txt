set(EDITOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../editor)
include_directories(${EDITOR_DIR} ${EDITOR_DIR}/contrib)
add_definitions(-DHEIMER_UNIT_TEST)

set(NAME SerializerTest)
set(SRC ${NAME}.cpp
    ${EDITOR_DIR}/draganddropstore.cpp
    ${EDITOR_DIR}/edge.cpp
    ${EDITOR_DIR}/edgebase.cpp
    ${EDITOR_DIR}/edgedot.cpp
    ${EDITOR_DIR}/edgetextedit.cpp
    ${EDITOR_DIR}/graph.cpp
    ${EDITOR_DIR}/graphicsfactory.cpp
    ${EDITOR_DIR}/hashseed.cpp
    ${EDITOR_DIR}/mindmapdata.cpp
    ${EDITOR_DIR}/mindmapdatabase.cpp
    ${EDITOR_DIR}/node.cpp
    ${EDITOR_DIR}/nodebase.cpp
    ${EDITOR_DIR}/nodehandle.cpp
    ${EDITOR_DIR}/serializer.cpp
    ${EDITOR_DIR}/textedit.cpp
    ${EDITOR_DIR}/contrib/mclogger.cc
    )

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(${NAME} ${SRC} ${MOC_SRC})
add_test(${NAME} ${CMAKE_SOURCE_DIR}/unittests/${NAME})

qt5_use_modules(${NAME} Test Xml Widgets)

set_property(TARGET ${NAME} PROPERTY CXX_STANDARD 11)
