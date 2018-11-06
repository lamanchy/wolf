cmake_minimum_required(VERSION 3.3)
project(wolf)

SET(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH "Build dir" FORCE)

set(CMAKE_CXX_STANDARD 11)
if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
elseif(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus -DBOOST_DATE_TIME_NO_LIB -DBOOST_REGEX_NO_LIB -DCPPKAFKA_STATIC -DLIBRDKAFKA_STATICLIB")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()


include_directories(include)
include_directories(src)
link_directories(lib)

set(SOURCE_FILES
        src/base/plugin.h
        src/base/plugin.cpp
        src/base/pipeline.h
        src/base/threaded_plugin.h
        src/whereami/whereami.c
        src/whereami/whereami.h
        src/plugins/generator.h src/plugins/cout.h src/plugins/tcp_in.h src/base/serializer.h src/serializers/line.h src/plugins/string_to_json.h src/base/json.h src/plugins/kafka_out.h src/base/mutexed_plugin.h src/plugins/collate.h src/plugins/ysoft/normalize_nlog_logs.h src/extras/convert_time.h src/plugins/ysoft/normalize_log4j2_logs.h src/plugins/ysoft/normalize_serilog_logs.h src/plugins/ysoft/add_local_info.h src/plugins/json_to_string.h src/plugins/tcp_out.h src/plugins/lambda.h src/extras/logger.h src/extras/logger.cpp)
add_executable(wolf src/main.cpp ${SOURCE_FILES})
# add_executable(kafka_benchmark src/kafka_benchmark.cpp ${SOURCE_FILES})


if(WIN32)
    set(LIBRARIES
            ${CMAKE_SOURCE_DIR}/lib/re2.lib
            ${CMAKE_SOURCE_DIR}/lib/cppkafka.lib
            ${CMAKE_SOURCE_DIR}/lib/rdkafka.lib
            ${CMAKE_SOURCE_DIR}/lib/zlibstatic.lib
            ${CMAKE_SOURCE_DIR}/lib/stxxl.lib
            ${CMAKE_SOURCE_DIR}/lib/rdkafka++.lib
            )
elseif(UNIX)
    set(LIBRARIES
            ${CMAKE_SOURCE_DIR}/lib/libre2.a
            ${CMAKE_SOURCE_DIR}/lib/libcppkafka.a
            ${CMAKE_SOURCE_DIR}/lib/librdkafka.so
            ${CMAKE_SOURCE_DIR}/lib/libstxxl_debug.a
            ${CMAKE_SOURCE_DIR}/lib/libz.a
            ${CMAKE_SOURCE_DIR}/lib/librdkafka++.a)
endif()

message(${LIBRARIES})

target_link_libraries(wolf ${LIBRARIES})
# target_link_libraries(kafka_benchmark ${LIBRARIES})

