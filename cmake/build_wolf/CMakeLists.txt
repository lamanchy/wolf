get_filename_component(WOLF_PATH ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
get_filename_component(WOLF_PATH ${WOLF_PATH} DIRECTORY)

# build libraries
include(${WOLF_PATH}/cmake/build_libraries.cmake)

# wolf library
include(${WOLF_PATH}/src/CMakeLists.txt)
include(${WOLF_PATH}/include/CMakeLists.txt)

# specify compile flags
include(${WOLF_PATH}/cmake/compile_flags.cmake)

# specify install definitions
include(${WOLF_PATH}/cmake/install_definitions.cmake)