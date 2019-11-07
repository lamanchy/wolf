# library path
set(WOLF_LIBS_PATH ${CMAKE_SOURCE_DIR}/lib/${WOLF_SYSTEM}-${WOLF_BUILD_SOURCE}/${CMAKE_BUILD_TYPE})

# build libraries in cofigure step
# libraries are build just once, if build is successful, it is not repeated
# clear cache to repeat build
if (NOT WOLF_SKIP_LIBRARY_BUILD)
    set(WOLF_LIBS_SOURCE ${CMAKE_SOURCE_DIR}/build/${WOLF_SYSTEM}-${WOLF_BUILD_SOURCE}/${CMAKE_BUILD_TYPE}/lib)
    file(MAKE_DIRECTORY ${WOLF_LIBS_SOURCE})
    execute_process(
            COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DWOLF_LIBS_PATH=${WOLF_LIBS_PATH} ${CMAKE_SOURCE_DIR}/lib
            WORKING_DIRECTORY ${WOLF_LIBS_SOURCE}
            RESULT_VARIABLE RESULT
    )
    if (NOT ${RESULT} EQUAL "0")
        message(FATAL_ERROR "Library build configuration failed")
    endif ()
    execute_process(
            COMMAND ${CMAKE_COMMAND} --build . --target all
            WORKING_DIRECTORY ${WOLF_LIBS_SOURCE}
            RESULT_VARIABLE RESULT
    )
    if (NOT ${RESULT} EQUAL "0")
        message(FATAL_ERROR "Library build failed")
    endif ()
    set(WOLF_SKIP_LIBRARY_BUILD ON CACHE BOOL "Set to skip building of libraries")
endif ()


# define libraries
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    if (WIN32)
        set(LIBRARIES re2.lib cppkafka.lib rdkafka.lib zlibstaticd.lib stxxl_debug.lib rdkafka++.lib)
    elseif (UNIX)
        set(LIBRARIES libre2.a libcppkafka.a librdkafka.a libstxxl_debug.a libz.a librdkafka++.a)
    endif ()
else ()
    if (WIN32)
        set(LIBRARIES re2.lib cppkafka.lib rdkafka.lib zlibstatic.lib stxxl.lib rdkafka++.lib)
    elseif (UNIX)
        set(LIBRARIES libre2.a libcppkafka.a librdkafka.a libstxxl.a libz.a librdkafka++.a)
    endif ()
endif ()
list(TRANSFORM LIBRARIES PREPEND ${WOLF_LIBS_PATH}/lib/)


# define flag for build
if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wl,--no-as-needed -ldl")
elseif (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus -DBOOST_DATE_TIME_NO_LIB -DBOOST_REGEX_NO_LIB -DCPPKAFKA_STATIC -DLIBRDKAFKA_STATICLIB -D_WIN32_WINNT=0x0501")
    list(TRANSFORM CMAKE_CXX_FLAGS_RELEASE REPLACE /MD /MT)
    list(TRANSFORM CMAKE_C_FLAGS_RELEASE REPLACE /MD /MT)
    list(TRANSFORM CMAKE_CXX_FLAGS_DEBUG REPLACE /MDd /MTd)
    list(TRANSFORM CMAKE_C_FLAGS_DEBUG REPLACE /MDd /MTd)
endif ()


# source files
set(SOURCE_FILES
        base/plugins/base_plugin.cpp
        base/pipeline.cpp
        base/plugins/threaded_plugin.cpp
        base/plugins/mutexed_threaded_plugin.cpp
        base/plugins/mutexed_plugin.cpp
        base/queue.cpp
        base/sleeper.cpp
        base/pipeline_status.cpp
        base/options/options.cpp

        plugins/deserializers/compressed.cpp
        plugins/deserializers/line.cpp
        plugins/deserializers/string.cpp
        plugins/serializers/compressed.cpp
        plugins/serializers/line.cpp
        plugins/serializers/string.cpp

        extras/logger.cpp
        extras/convert_time.cpp
        extras/get_executable_path.cpp
        extras/get_time.cpp
        extras/gzip.cpp

        libs/whereami/whereami.c
        )
list(TRANSFORM SOURCE_FILES PREPEND ${CMAKE_SOURCE_DIR}/wolf/)

add_library(wolf STATIC ${SOURCE_FILES})

target_include_directories(wolf PUBLIC ${CMAKE_SOURCE_DIR}/wolf)
target_include_directories(wolf SYSTEM PUBLIC ${WOLF_LIBS_PATH}/include)
target_link_libraries(wolf ${LIBRARIES})