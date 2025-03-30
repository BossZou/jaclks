aux_source_directory(./hash TEST_SRC_FILES)

add_executable(tests main.cc ${TEST_SRC_FILES})
