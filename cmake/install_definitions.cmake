install(TARGETS wolf)

install(CODE "
if (NOT EXISTS ${CMAKE_INSTALL_PREFIX}/include)
    message(STATUS \"installing libraries, only done for the first time\")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${WOLF_LIBS_PATH}/include ${CMAKE_INSTALL_PREFIX}/include)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${WOLF_LIBS_PATH}/LIB ${CMAKE_INSTALL_PREFIX}/LIB)
else()
    message(STATUS \"skipping libraries install, only done for the first time\")
endif()
")

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
        DESTINATION .
        MESSAGE_NEVER
        FILES_MATCHING PATTERN "*.h"
        )