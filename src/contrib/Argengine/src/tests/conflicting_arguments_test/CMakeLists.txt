set(ARGENGINE_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories(${ARGENGINE} ${CMAKE_CURRENT_SOURCE_DIR})

set(NAME conflicting_arguments_test)
set(SRC ${NAME}.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/tests)
add_executable(${NAME} ${SRC})
add_test(${NAME} ${CMAKE_BINARY_DIR}/tests/${NAME})
target_link_libraries(${NAME} ${LIBRARY_NAME})
