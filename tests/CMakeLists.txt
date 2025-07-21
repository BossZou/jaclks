aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/function TEST_SRC_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/hash TEST_SRC_FILES)

add_executable(tests main.cc
        ${TEST_SRC_FILES}
)

target_link_libraries(tests PRIVATE
        gtest
        gtest_main
)

add_dependencies(tests build_thirdparty)

# Registers googletests
add_test(NAME google_tests COMMAND tests)
