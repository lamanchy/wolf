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
        src/base/plugins/plugin.h src/base/plugins/plugin.cpp src/base/pipeline.h src/base/pipeline.cpp
        src/base/plugins/threaded_plugin.h src/whereami/whereami.c src/whereami/whereami.h
        src/plugins/generator.h src/plugins/cout.h src/plugins/tcp_in.h src/base/serializer.h src/serializers/line.h
        src/plugins/string_to_json.h src/base/json.h src/plugins/kafka_out.h src/base/plugins/mutexed_plugin.h
        src/plugins/collate.h src/plugins/ysoft/normalize_nlog_logs.h src/extras/convert_time.h
        src/plugins/ysoft/normalize_log4j2_logs.h src/plugins/ysoft/normalize_serilog_logs.h
        src/plugins/ysoft/add_local_info.h src/plugins/json_to_string.h src/plugins/tcp_out.h src/plugins/lambda.h
        src/extras/logger.h src/extras/logger.cpp src/plugins/kafka_in.h src/plugins/http_out.h src/plugins/stats.h
        src/base/plugins/mutexed_threaded_plugin.h src/extras/get_time.h src/plugins/stream_sort.h
        src/extras/get_executable_path.h src/plugins/elapsed.h src/plugins/cin.h src/base/options/event_option.h
        src/base/options/event.h src/base/options/command.h src/plugins/regex.h
        src/plugins/ysoft/get_elapsed_preevents.h src/plugins/ysoft/count_logs.h src/serializers/plain.h
        src/plugins/json_to_influx.h src/plugins/filter.h src/plugins/drop.h
        src/serializers/compressed.h src/serializers/serialize.h src/serializers/deserialize.h
        src/base/plugins/threaded_plugin.cpp src/base/plugins/threaded_plugin.h
        src/base/plugins/mutexed_threaded_plugin.cpp src/base/plugins/mutexed_threaded_plugin.h
        src/base/plugins/mutexed_plugin.cpp src/base/plugins/mutexed_plugin.h src/serializers/compressed.cpp
        src/serializers/compressed.h src/base/options/event_option.cpp src/base/options/event_option.h src/extras/convert_time.cpp
        src/extras/convert_time.h src/extras/get_executable_path.cpp src/extras/get_executable_path.h
        src/extras/get_time.cpp src/extras/get_time.h src/serializers/line.cpp src/serializers/line.h
        src/plugins/time_sort.h src/plugins/file_out.h src/plugins/file_in.h src/plugins/copy.h src/extras/validators.h src/wolf.h)


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

include_directories(lib/${LIB_PATH}/${CMAKE_BUILD_TYPE}/include)
link_directories(lib/${LIB_PATH}/${CMAKE_BUILD_TYPE}/lib)
include_directories(src)

set(TARGETS calf_parser metrics_forwarder correlator parser little_stack_parser forwarder)

foreach (executable ${TARGETS})
    add_executable(${executable}${TARGET_POSTFIX} src/${executable}.cpp ${SOURCE_FILES})
    target_link_libraries(${executable}${TARGET_POSTFIX} ${LIBRARIES})
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
