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


include_directories(build-${CMAKE_BUILD_TYPE}/include)
include_directories(include)
include_directories(src)
link_directories(build-${CMAKE_BUILD_TYPE}/lib)

set(SOURCE_FILES
        src/base/plugins/plugin.h
        src/base/plugins/plugin.cpp
        src/base/pipeline.h
        src/base/plugins/threaded_plugin.h
        src/whereami/whereami.c
        src/whereami/whereami.h
        src/plugins/generator.h src/plugins/cout.h src/plugins/tcp_in.h src/base/serializer.h src/serializers/line.h src/plugins/string_to_json.h src/base/json.h src/plugins/kafka_out.h src/base/plugins/mutexed_plugin.h src/plugins/collate.h src/plugins/ysoft/normalize_nlog_logs.h src/extras/convert_time.h src/plugins/ysoft/normalize_log4j2_logs.h src/plugins/ysoft/normalize_serilog_logs.h src/plugins/ysoft/add_local_info.h src/plugins/json_to_string.h src/plugins/tcp_out.h src/plugins/lambda.h src/extras/logger.h src/extras/logger.cpp src/plugins/kafka_in.h src/plugins/http_out.h src/plugins/stats.h src/base/plugins/mutexed_threaded_plugin.h src/extras/get_time.h src/plugins/stream_sort.h src/extras/get_executable_path.h src/plugins/elapsed.h src/plugins/cin.h src/base/options/option.h src/base/options/constant.h src/base/options/event.h src/base/options/command.h src/plugins/regex.h src/plugins/ysoft/get_elapsed_preevents.h src/plugins/ysoft/count_logs.h)
# add_executable(kafka_benchmark src/kafka_benchmark.cpp ${SOURCE_FILES})


if (WIN32)
    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        set(LIBRARIES
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/re2.lib
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/cppkafka.lib
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/rdkafka.lib
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/zlibstaticd.lib
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/stxxl_debug.lib
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/rdkafka++.lib
                )
        set(POSTFIX windows-debug)
    else ()
        set(LIBRARIES
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/re2.lib
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/cppkafka.lib
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/rdkafka.lib
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/zlibstatic.lib
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/stxxl.lib
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/rdkafka++.lib
                )
        set(POSTFIX windows)
    endif ()
elseif (UNIX)
    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        set(LIBRARIES
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/libre2.a
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/libcppkafka.a
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/librdkafka.a
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/libstxxl_debug.a
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/libz.a
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/librdkafka++.a
                )
        set(POSTFIX linux-debug)
    else ()
        set(LIBRARIES
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/libre2.a
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/libcppkafka.a
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/librdkafka.a
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/libstxxl.a
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/libz.a
                ${CMAKE_SOURCE_DIR}/build-${CMAKE_BUILD_TYPE}/lib/librdkafka++.a
                )
        set(POSTFIX linux)
    endif ()
endif ()

set(TARGETS wolf metrics_forwarder correlator parser)

foreach (executable ${TARGETS})
    add_executable(${executable} src/${executable}.cpp ${SOURCE_FILES})
    target_link_libraries(${executable} ${LIBRARIES})
endforeach ()

foreach(target ${TARGETS})
    install(TARGETS ${target} DESTINATION ${target}-${POSTFIX}/)
endforeach()
