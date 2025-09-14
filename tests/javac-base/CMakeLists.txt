# add all test files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/java/lang JBASE_TEST_SRC_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/java/security
                     JBASE_TEST_SRC_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/java/util/regex
                     JBASE_TEST_SRC_FILES)

add_executable(jbase_gtest main.cc ${JBASE_TEST_SRC_FILES})

target_link_libraries(jbase_gtest PRIVATE MyThirdParty::All jaclks_static
                                          GTest::gtest)

add_dependencies(jbase_gtest jaclks_static)

# Registers googletests
add_test(NAME jbase_gtest COMMAND $<TARGET_FILE:jbase_gtest>)
