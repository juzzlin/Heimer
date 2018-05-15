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

# Global game version
set(VERSION_MAJOR "0")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

option(ReleaseBuild "This should be used when creating DEB and RPM packages." OFF)

option(DATA_PATH "Optional DATA_PATH for Linux release build." "")

option(BIN_PATH "Optional BIN_PATH for Linux release build." "")

option(DOC_PATH "Optional DOC_PATH for Linux release build." "")

# Default to release C++ flags if CMAKE_BUILD_TYPE not set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo
MinSizeRel." FORCE)
endif()

if(UNIX)
    include("InstallLinux.cmake")
elseif(WIN32)
    include("InstallWindows.cmake")
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
elseif(MSVC)
    add_definitions(-DNOMINMAX)
endif()

set(BINARY_NAME "heimer")

add_definitions(-DVERSION="${VERSION}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(QT_MIN_VER 5.2.1)
find_package(Qt5Core ${QT_MIN_VER} REQUIRED)
find_package(Qt5Xml ${QT_MIN_VER} REQUIRED)
find_package(Qt5Widgets ${QT_MIN_VER} REQUIRED)
find_package(Qt5LinguistTools ${QT_MIN_VER} REQUIRED)
find_package(Qt5Test ${QT_MIN_VER} REQUIRED)

# Install paths depend on the build type and target platform
resolve_install_paths()

add_subdirectory(src/editor)

# Enable CMake's unit test framework
enable_testing()
add_subdirectory(src/UnitTests)
