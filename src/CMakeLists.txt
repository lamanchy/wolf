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
list(TRANSFORM SOURCE_FILES PREPEND ${WOLF_PATH}/src/)

add_library(wolf STATIC ${SOURCE_FILES})