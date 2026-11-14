# Qt 6 requires CMake 3.16
cmake_minimum_required(VERSION 3.16)

project(Heimer)

# Global version
set(APPLICATION_VERSION_MAJOR 4)
set(APPLICATION_VERSION_MINOR 5)
set(APPLICATION_VERSION_PATCH 0)
set(APPLICATION_VERSION "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}.${APPLICATION_VERSION_PATCH}")

message("\n${CMAKE_PROJECT_NAME} ${APPLICATION_VERSION}\n")
message(STATUS "System name       : ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "System version    : ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Compiler identity : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler version  : ${CMAKE_CXX_COMPILER_VERSION}")

# Some common CPack variables
set(CPACK_PACKAGE_VERSION_MAJOR ${APPLICATION_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${APPLICATION_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${APPLICATION_VERSION_PATCH})

set(CPACK_PACKAGE_NAME heimer)
set(CPACK_PACKAGE_VENDOR Juzzlin)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Heimer is a simple cross-platform mind map tool written in Qt.")
set(CPACK_PACKAGE_ICON ${CMAKE_SOURCE_DIR}/data/images/about.png)

set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)

option(BIN_PATH "Optional BIN_PATH for Linux release build." "")

option(BUILD_TESTS "Build unit tests." ON)

option(ENABLE_CCACHE "Use CCache if found." ON)

# Default to release C++ flags if CMAKE_BUILD_TYPE not set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo
MinSizeRel." FORCE)
endif()
message(STATUS "Build type        : ${CMAKE_BUILD_TYPE}")

# Package type info shown in the About dialog
if(PACKAGE_TYPE)
    add_definitions(-DPACKAGE_TYPE="${PACKAGE_TYPE}")
endif()

if(UNIX)
    include("InstallLinux.cmake")
elseif(WIN32)
    include("InstallWindows.cmake")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    # Automatically use ccache if found
    if(ENABLE_CCACHE)
        find_program(CCACHE_FOUND ccache)
        if(CCACHE_FOUND)
            set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
            set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
        endif()
        message(STATUS "Ccache            : ${CCACHE_FOUND}")
    endif()
elseif(MSVC)
    add_definitions(-DNOMINMAX)
endif()

string(TOLOWER ${CMAKE_PROJECT_NAME} BINARY_NAME)
set(HEIMER_LIB_NAME ${CMAKE_PROJECT_NAME}_static) # See GitHub Issue #259

add_definitions(-DAPPLICATION_NAME="${CMAKE_PROJECT_NAME}")
add_definitions(-DAPPLICATION_VERSION="${APPLICATION_VERSION}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Minimum supported Qt version: the version in Ubuntu 22.04 LTS
find_package(Qt6 6.2.4 REQUIRED COMPONENTS Core Xml Widgets LinguistTools Svg Test Network)
message(STATUS "Qt version found  : ${Qt6_VERSION}\n")

# Install paths depend on the build type and target platform
setup_install_targets()

add_subdirectory(src)

# Enable CMake's unit test framework
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/unit_tests)
endif()

