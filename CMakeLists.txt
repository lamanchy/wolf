cmake_minimum_required(VERSION 3.3)
project(wolf)

set(CMAKE_CXX_STANDARD 11)
if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wl,--no-as-needed -ldl")
elseif (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus -DBOOST_DATE_TIME_NO_LIB -DBOOST_REGEX_NO_LIB -DCPPKAFKA_STATIC -DLIBRDKAFKA_STATICLIB")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif ()

message(Building libraries)
if(NOT WOLF_BUILD_SOURCE)
    set(WOLF_BUILD_SOURCE local)
endif()

file(LOCK ${CMAKE_SOURCE_DIR}/lib DIRECTORY)
if (WIN32)
    execute_process(COMMAND python ${CMAKE_SOURCE_DIR}/install_libraries.py ${WOLF_BUILD_SOURCE})
    set(LIB_PATH win-${WOLF_BUILD_SOURCE})
elseif (UNIX)
    execute_process(COMMAND ${CMAKE_SOURCE_DIR}/install_libraries.py ${WOLF_BUILD_SOURCE})
    set(LIB_PATH linux-${WOLF_BUILD_SOURCE})
endif()
file(LOCK ${CMAKE_SOURCE_DIR}/lib DIRECTORY RELEASE)

set(SOURCE_FILES
        src/base/plugins/base_plugin.cpp
        src/base/pipeline.cpp
        src/whereami/whereami.c
        src/extras/logger.cpp
        src/base/plugins/threaded_plugin.cpp
        src/base/plugins/mutexed_threaded_plugin.cpp
        src/base/plugins/mutexed_plugin.cpp
        src/extras/convert_time.cpp
        src/extras/get_executable_path.cpp
        src/extras/get_time.cpp
        src/base/queue.cpp
        src/base/sleeper.cpp
        src/base/pipeline_status.cpp
        src/base/options/options.cpp
        src/plugins/deserializers/compressed.cpp
        src/plugins/deserializers/line.cpp
        src/plugins/deserializers/string.cpp
        src/plugins/serializers/compressed.cpp
        src/plugins/serializers/line.cpp
        src/plugins/serializers/string.cpp
        src/extras/gzip.cpp)

add_library(wolf STATIC ${SOURCE_FILES})


if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    if (WIN32)
        set(LIBRARIES re2.lib cppkafka.lib rdkafka.lib zlibstaticd.lib stxxl_debug.lib rdkafka++.lib)
        set(DIR_POSTFIX windows-debug)
    elseif (UNIX)
        set(LIBRARIES libre2.a libcppkafka.a librdkafka.a libstxxl_debug.a libz.a librdkafka++.a)
        set(DIR_POSTFIX linux-debug)
    endif ()
else ()
    if (WIN32)
        set(LIBRARIES re2.lib cppkafka.lib rdkafka.lib zlibstatic.lib stxxl.lib rdkafka++.lib)
        set(DIR_POSTFIX windows)
    elseif (UNIX)
        set(LIBRARIES libre2.a libcppkafka.a librdkafka.a libstxxl.a libz.a librdkafka++.a)
        set(DIR_POSTFIX linux)
    endif ()
endif ()

list(TRANSFORM LIBRARIES PREPEND ${CMAKE_SOURCE_DIR}/lib/${LIB_PATH}/${CMAKE_BUILD_TYPE}/lib/)

include_directories(SYSTEM lib/${LIB_PATH}/${CMAKE_BUILD_TYPE}/include)
link_directories(lib/${LIB_PATH}/${CMAKE_BUILD_TYPE}/lib)
include_directories(src)

set(TARGETS calf_parser metrics_forwarder correlator parser little_stack_parser forwarder gzip_test)

target_link_libraries(wolf ${LIBRARIES})

foreach (executable ${TARGETS})
    add_executable(${executable}${TARGET_POSTFIX} src/${executable}.cpp)
    target_link_libraries(${executable}${TARGET_POSTFIX} wolf)
endforeach ()

foreach (target ${TARGETS})
    install(TARGETS ${target}${TARGET_POSTFIX} DESTINATION ${target}-${DIR_POSTFIX}/)
    if (${target} STREQUAL parser)
        install(FILES configs/parsers DESTINATION ${target}-${DIR_POSTFIX}/)
        install(FILES configs/elapsed DESTINATION ${target}-${DIR_POSTFIX}/)
    endif ()
    if (${target} STREQUAL little_stack_parser)
        install(FILES configs/parsers DESTINATION ${target}-${DIR_POSTFIX}/)
        install(FILES configs/elapsed DESTINATION ${target}-${DIR_POSTFIX}/)
    endif ()
endforeach ()
