target_include_directories(wolf PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(wolf SYSTEM PRIVATE ${WOLF_LIBS_PATH}/include)
target_link_directories(wolf PRIVATE ${WOLF_LIBS_PATH}/lib)