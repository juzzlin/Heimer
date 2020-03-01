set(SRC argengine.cpp)
set(HDR argengine.hpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(ArgengineLib OBJECT ${SRC})
set_property(TARGET ArgengineLib PROPERTY POSITION_INDEPENDENT_CODE 1)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
if(${ARGENGINE_SHARED_LIB})
    add_library(${LIBRARY_NAME} SHARED $<TARGET_OBJECTS:ArgengineLib>)
else()
    add_library(${LIBRARY_NAME} STATIC $<TARGET_OBJECTS:ArgengineLib>)
endif()
set_target_properties(${LIBRARY_NAME} PROPERTIES VERSION ${VERSION} SOVERSION ${VERSION_MAJOR})
set_target_properties(${LIBRARY_NAME} PROPERTIES PUBLIC_HEADER ${HDR})

install(TARGETS ${LIBRARY_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

