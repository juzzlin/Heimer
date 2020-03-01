set(ARGENGINE_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories(${ARGENGINE} ${CMAKE_CURRENT_SOURCE_DIR})

set(NAME ex1)
set(SRC ${NAME}.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
add_executable(${NAME} ${SRC})
target_link_libraries(${NAME} ${LIBRARY_NAME})

