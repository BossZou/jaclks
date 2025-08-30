# add all src files set(JBASE_SRC_ROOT_DIR
# ${PROJECT_ROOT_DIR}/src/jaclks-internal/javac-base)
# aux_source_directory(${JBASE_SRC_ROOT_DIR}/java/lang JBASE_SRC_FILES)
# aux_source_directory(${JBASE_SRC_ROOT_DIR}/java/security JBASE_SRC_FILES)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
if(PLATFORM_WINDOWS)
  link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

# add all test files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/java/lang JBASE_TEST_SRC_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/java/security
                     JBASE_TEST_SRC_FILES)

add_executable(jbase_gtest main.cc ${JBASE_SRC_FILES} ${JBASE_TEST_SRC_FILES})

target_link_libraries(jbase_gtest PRIVATE gtest jaclks_shared)

add_dependencies(jbase_gtest build_thirdparty jaclks_shared)

# Registers googletests
add_test(NAME jbase_gtest COMMAND $<TARGET_FILE:jbase_gtest>)
