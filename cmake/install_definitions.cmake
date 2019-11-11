install(TARGETS wolf)

install(CODE "
if (NOT EXISTS ${CMAKE_INSTALL_PREFIX}/include)
    message(STATUS \"installing libraries, only done for the first time\")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${WOLF_LIBS_PATH}/include ${CMAKE_INSTALL_PREFIX}/include)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${WOLF_LIBS_PATH}/lib ${CMAKE_INSTALL_PREFIX}/lib)
else()
    message(STATUS \"skipping libraries install, only done for the first time\")
endif()
")

install(DIRECTORY ${WOLF_PATH}/include
        DESTINATION .
        MESSAGE_NEVER
        FILES_MATCHING PATTERN "*.h"
        )