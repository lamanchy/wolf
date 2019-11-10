cmake_minimum_required(VERSION 3.13)
project(wolf)
set(CMAKE_CXX_STANDARD 11)

# check for current platform
if (WIN32)
    set(WOLF_SYSTEM windows)
elseif (UNIX)
    set(WOLF_SYSTEM linux)
else ()
    message(FATAL_ERROR "Unknown system")
endif ()

# build libraries
include(cmake/build_libraries.cmake)

# wolf library
include(src/CMakeLists.txt)
include(include/CMakeLists.txt)

# specify compile flags
include(cmake/compile_flags.cmake)

# specify install definitions
include(cmake/install_definitions.cmake)