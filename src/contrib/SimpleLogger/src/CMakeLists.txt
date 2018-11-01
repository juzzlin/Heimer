set(SRC simple_logger.cpp)
set(HDR simple_logger.hpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# This is the "object library" target: compiles the sources only once
add_library(objlib OBJECT ${SRC})

# Shared libraries need PIC
set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE 1)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
if(${SIMPLE_LOGGER_SHARED_LIB})
    add_library(${LIBRARY_NAME} SHARED $<TARGET_OBJECTS:objlib>)
else()
    add_library(${LIBRARY_NAME} STATIC $<TARGET_OBJECTS:objlib>)
endif()
set_target_properties(${LIBRARY_NAME} PROPERTIES VERSION ${VERSION} SOVERSION ${VERSION_MAJOR})
set_target_properties(${LIBRARY_NAME} PROPERTIES PUBLIC_HEADER ${HDR})
