include_directories(${PROJECT_ROOT_DIR}/src)

# add all test files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/utils JCXX_TEST_SRC_FILES)

add_executable(jcxx_gtest main.cc ${JCXX_TEST_SRC_FILES})

target_link_libraries(jcxx_gtest PRIVATE MyThirdParty::All jaclks_static
                                         GTest::gtest)

add_dependencies(jcxx_gtest jaclks_static)

# Registers googletests
add_test(NAME jcxx_gtest COMMAND $<TARGET_FILE:jcxx_gtest>)
