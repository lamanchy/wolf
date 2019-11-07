cmake_minimum_required(VERSION 3.12)
project(wolf)
set(CMAKE_CXX_STANDARD 11)

# set wolf_build_source if building from docker or wsl
# to allow multiple builds inside one directory
if (NOT WOLF_BUILD_SOURCE)
    set(WOLF_BUILD_SOURCE local)
endif ()

# check for current platform
if (WIN32)
    set(WOLF_SYSTEM windows)
elseif (UNIX)
    set(WOLF_SYSTEM linux)
else ()
    message(FATAL_ERROR "Unknown system")
endif ()

# wolf library
include(wolf/CMakeLists.txt)

# executables
add_subdirectory(src)
