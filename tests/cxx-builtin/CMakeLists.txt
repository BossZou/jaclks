# add all test files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/builtin CXX_TEST_SRC_FILES)

add_executable(cxx_gtest ${CMAKE_CURRENT_SOURCE_DIR}/main.cc ${CXX_TEST_SRC_FILES})

target_link_libraries(cxx_gtest PRIVATE MyThirdParty::All jaclks_static
                                        GTest::gtest)

add_dependencies(cxx_gtest jaclks_static)

# Registers googletests
add_test(NAME cxx_gtest COMMAND $<TARGET_FILE:cxx_gtest>)
