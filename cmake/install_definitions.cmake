install(TARGETS wolf)

install(
        DIRECTORY ${CMAKE_SOURCE_DIR}/include
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")