# add all src files
aux_source_directory(${PROJECT_SOURCE_DIR}/src/javac-base JBASE_SRC_FILES)

# add all test files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} JBASE_TEST_SRC_FILES)

add_executable(jbase_gtest ${JBASE_SRC_FILES} ${JBASE_TEST_SRC_FILES})

target_link_libraries(jbase_gtest PRIVATE gtest)

add_dependencies(jbase_gtest build_thirdparty)

# Registers googletests
add_test(NAME jbase_gtest COMMAND $<TARGET_FILE:jbase_gtest>)
