set(EDITOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../editor)
include_directories(${EDITOR_DIR})

set(NAME GraphTest)
set(SRC ${NAME}.cpp ${EDITOR_DIR}/graph.cpp ${EDITOR_DIR}/nodebase.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/unittests)
add_executable(${NAME} ${SRC} ${MOC_SRC})
add_test(${NAME} ${CMAKE_SOURCE_DIR}/unittests/${NAME})

qt5_use_modules(${NAME} Test)

set_property(TARGET ${NAME} PROPERTY CXX_STANDARD 11)
