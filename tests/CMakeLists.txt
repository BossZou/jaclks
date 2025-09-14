# add all src files

# add all test files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/function TEST_SRC_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/hash TEST_SRC_FILES)

add_executable(
  jaclks_gtest
  main.cc ${SRC_FILES} ${TEST_SRC_FILES}
  ../include/jaclks/javac-base/java/lang/index_out_of_bounds_exception.h)

target_link_libraries(jaclks_gtest PRIVATE GTest::gtest)

add_subdirectory(javac-base)

# Registers googletests
add_test(NAME jaclks_gtest COMMAND $<TARGET_FILE:jaclks_gtest>)
