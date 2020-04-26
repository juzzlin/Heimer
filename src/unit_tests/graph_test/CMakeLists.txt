set(EDITOR_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories(${EDITOR_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DHEIMER_UNIT_TEST)

set(NAME graph_test)
set(SRC
    ${NAME}.cpp
    ${EDITOR_DIR}/edge.cpp
    ${EDITOR_DIR}/edge_dot.cpp
    ${EDITOR_DIR}/edge_text_edit.cpp
    ${EDITOR_DIR}/graph.cpp
    ${EDITOR_DIR}/graphics_factory.cpp
    ${EDITOR_DIR}/image.cpp
    ${EDITOR_DIR}/node.cpp
    ${EDITOR_DIR}/node_handle.cpp
    ${EDITOR_DIR}/text_edit.cpp
    )

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/unit_tests)
add_executable(${NAME} ${SRC} ${MOC_SRC})
add_test(${NAME} ${CMAKE_BINARY_DIR}/unit_tests/${NAME})
target_link_libraries(${NAME} Qt5::Test Qt5::Widgets SimpleLogger_static)
