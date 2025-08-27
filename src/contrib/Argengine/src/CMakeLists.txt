set(SRC argengine.cpp)
set(HDR argengine.hpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(ArgengineLib OBJECT ${HDR} ${SRC})
set_property(TARGET ArgengineLib PROPERTY POSITION_INDEPENDENT_CODE 1)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_library(${LIBRARY_NAME} SHARED $<TARGET_OBJECTS:ArgengineLib>)
set_target_properties(${LIBRARY_NAME} PROPERTIES PUBLIC_HEADER ${HDR})
install(TARGETS ${LIBRARY_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

set(STATIC_LIBRARY_NAME ${LIBRARY_NAME}_static)
add_library(${STATIC_LIBRARY_NAME} STATIC $<TARGET_OBJECTS:ArgengineLib>)
set_target_properties(${STATIC_LIBRARY_NAME} PROPERTIES PUBLIC_HEADER ${HDR})
install(TARGETS ${STATIC_LIBRARY_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

