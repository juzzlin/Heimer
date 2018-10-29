set(EDITOR_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories(${EDITOR_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DHEIMER_UNIT_TEST)

set(NAME graphtest)
set(SRC ${NAME}.cpp ${EDITOR_DIR}/edgebase.cpp ${EDITOR_DIR}/graph.cpp ${EDITOR_DIR}/nodebase.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/unittests)
add_executable(${NAME} ${SRC} ${MOC_SRC})
add_test(${NAME} ${CMAKE_BINARY_DIR}/unittests/${NAME})
target_link_libraries(${NAME} Qt5::Test Qt5::Widgets SimpleLogger)
