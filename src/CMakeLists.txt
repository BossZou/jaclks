aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/concurrent SRC_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/string SRC_FILES)

add_library(jaclks SHARED ${SRC_FILES})
