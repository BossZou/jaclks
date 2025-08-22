# add all src files
aux_source_directory(${PROJECT_SOURCE_DIR}/src/concurrent SRC_FILES)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/lang SRC_FILES)

# add all test files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/concurrent TEST_SRC_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/function TEST_SRC_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/hash TEST_SRC_FILES)

add_executable(jaclks_gtest main.cc ${SRC_FILES} ${TEST_SRC_FILES})

target_link_libraries(jaclks_gtest PRIVATE gtest gtest_main)

add_dependencies(jaclks_gtest build_thirdparty)

add_subdirectory(javac-base)

# Registers googletests
add_test(NAME jaclks_gtest COMMAND $<TARGET_FILE:jaclks_gtest>)
