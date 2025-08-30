# add all src files set(JBASE_SRC_ROOT_DIR
# ${PROJECT_ROOT_DIR}/src/jaclks-internal/javac-base)
# aux_source_directory(${JBASE_SRC_ROOT_DIR}/java/lang JBASE_SRC_FILES)
# aux_source_directory(${JBASE_SRC_ROOT_DIR}/java/security JBASE_SRC_FILES)

# if(PLATFORM_WINDOWS)
# link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
# link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}) else()
# link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY}) endif()

# add all test files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/java/lang JBASE_TEST_SRC_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/java/security
                     JBASE_TEST_SRC_FILES)

add_executable(jbase_gtest main.cc ${JBASE_TEST_SRC_FILES})

target_link_libraries(jbase_gtest PRIVATE jaclks_static gtest)

add_dependencies(jbase_gtest build_thirdparty jaclks_static)

add_custom_command(
  TARGET jbase_gtest
  PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:jaclks_shared>
          $<TARGET_FILE_DIR:jbase_gtest>
  COMMENT "Copying DLL to executable directory")

# Registers googletests
add_test(NAME jbase_gtest COMMAND $<TARGET_FILE:jbase_gtest>)
