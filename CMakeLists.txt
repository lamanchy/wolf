cmake_minimum_required(VERSION 3.13)
project(wolf)
set(CMAKE_CXX_STANDARD 11)

# build libraries
include(cmake/build_libraries.cmake)

# wolf library
include(src/CMakeLists.txt)
include(include/CMakeLists.txt)

# specify compile flags
include(cmake/compile_flags.cmake)

# specify install definitions
include(cmake/install_definitions.cmake)