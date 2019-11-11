# library path
set(WOLF_LIBS_PATH ${CMAKE_CURRENT_BINARY_DIR}/libs)

# build libraries in cofigure step
# libraries are build just once, if build is successful, it is not repeated
# clear cache to repeat build
if (NOT WOLF_SKIP_LIBRARY_BUILD)
    set(WOLF_LIBS_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/lib_source)
    file(MAKE_DIRECTORY ${WOLF_LIBS_SOURCE})
    execute_process(
            COMMAND ${CMAKE_COMMAND}
            -G ${CMAKE_GENERATOR}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DWOLF_LIBS_PATH=${WOLF_LIBS_PATH}
            ${WOLF_PATH}/cmake/libs

            WORKING_DIRECTORY ${WOLF_LIBS_SOURCE}
            RESULT_VARIABLE RESULT
    )
    if (NOT ${RESULT} EQUAL "0")
        message(FATAL_ERROR "Library build configuration failed")
    endif ()
    execute_process(
            COMMAND ${CMAKE_COMMAND} --build . --target all -j 4
            WORKING_DIRECTORY ${WOLF_LIBS_SOURCE}
            RESULT_VARIABLE RESULT
    )
    if (NOT ${RESULT} EQUAL "0")
        message(FATAL_ERROR "Library build failed")
    endif ()
    set(WOLF_SKIP_LIBRARY_BUILD ON CACHE BOOL "Set to skip building of libraries")
endif ()
