install(TARGETS wolf)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
        DESTINATION .
        FILES_MATCHING PATTERN "*.h")