project(Heimer)

cmake_minimum_required(VERSION 2.8.12)
cmake_policy(VERSION 2.8.12)

if(POLICY CMP0005)
    cmake_policy(SET CMP0005 NEW)
endif()

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0043)
    cmake_policy(SET CMP0043 NEW)
endif()

if(POLICY CMP0071)
    cmake_policy(SET CMP0071 NEW)
endif()

# Default to GLVND
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

# Global version
set(VERSION_MAJOR 3)
set(VERSION_MINOR 3)
set(VERSION_PATCH 0)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# Some common CPack variables
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})

set(CPACK_PACKAGE_NAME heimer)
set(CPACK_PACKAGE_VENDOR Juzzlin)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Heimer is a simple cross-platform mind map tool written in Qt.")
set(CPACK_PACKAGE_ICON ${CMAKE_SOURCE_DIR}/data/images/about.png)

set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)

option(BIN_PATH "Optional BIN_PATH for Linux release build." "")

option(BUILD_TESTS "Build unit tests." ON)

# Default to release C++ flags if CMAKE_BUILD_TYPE not set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo
MinSizeRel." FORCE)
endif()

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

    # CMAKE_CXX_STANDARD supported only by versions >= 3.1
    if (CMAKE_VERSION VERSION_LESS "3.1")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    endif ()

    # Automatically use ccache if found
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    endif(CCACHE_FOUND)

elseif(MSVC)
    add_definitions(-DNOMINMAX)
endif()

set(BINARY_NAME "heimer")
set(LIBRARY_NAME "HeimerLib")

add_definitions(-DVERSION="${VERSION}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(QT_MIN_VER 5.5.1) # The version in Ubuntu 16.04
# This is what Qt Creator 4.13.2 would generate
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Xml Widgets LinguistTools Svg Test Network REQUIRED)

# Install paths depend on the build type and target platform
setup_install_targets()

add_subdirectory(src)

# Enable CMake's unit test framework
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/unit_tests)
endif()

