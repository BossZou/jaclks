cmake_minimum_required(VERSION 3.12)

project(
  Jaclks
  LANGUAGES C CXX
  VERSION 0.0.1)

# 全局标准设置
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Set output directory for static libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Set output directory for shared libraries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR})

option(ENABLE_GTEST "Enable build googletest executable" OFF)
option(ENABLE_COVERAGE "Enable build project with coverage options" OFF)

# Initial and update git submodule
execute_process(
  COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  RESULT_VARIABLE GIT_SUBMOD_RESULT)

include(cmake/DetectPlatform.cmake)
include(cmake/HandleDependencies.cmake)

if(BYTE_ORDER_BIG_ENDIAN)
  add_definitions(-DJACLKS_BIG_ENDIAN)
else()
  add_definitions(-DJACLKS_LITTLE_ENDIAN)
endif()

if(PLATFORM_WINDOWS)
  if(MSVC)
    add_definitions(-DJACLKS_OS_WINDOWS)
    add_compile_options(/W4 /WX) # /W4 warning level, /WX regard warning as
                                 # error
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
  else()
    message(
      FATAL_ERROR
        "Unsupported C compiler ${CMAKE_C_COMPILER} or C++ compiler ${CMAKE_CXX_COMPILER}"
    )
  endif()
elseif(PLATFORM_LINUX OR PLATFORM_MACOS)
  # GCC/Clang compiler options
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -pthread -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -pthread -fPIC")
  if(PLATFORM_LINUX)
    add_definitions(-DJACLKS_OS_LINUX)
  else()
    add_definitions(-DJACLKS_OS_MACOS)
  endif()
else()
  message(FATAL_ERROR "Unsupported OS platform ${CMAKE_SYSTEM_NAME}")
endif()

if(ENABLE_GTEST AND ENABLE_COVERAGE)
  if(PLATFORM_WINDOWS)
    message(
      WARNING
        "Code coverage is not properly supported with MSVC in this configuration"
    )
  else()
    message(STATUS "Compile code with coverage option")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -O0 -g -fprofile-arcs -ftest-coverage")
    add_link_options(--coverage)
  endif()
endif()

message(STATUS "CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")

# Third party configurations
if(MSVC)
  add_custom_target(
    build_thirdparty
    COMMAND cmake -E make_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest-build
    COMMAND
      cd ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest-build && cmake
      ../googletest -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/dist/third-party/googletest
      -DBUILD_SHARED_LIBS=OFF -Dgtest_force_shared_crt=ON -G "Ninja" && cmake
      --build . --target install
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building GTest with Ninja")
else()
  add_custom_target(
    build_thirdparty
    COMMAND ${CMAKE_MAKE_PROGRAM} gtest -f ${CMAKE_CURRENT_SOURCE_DIR}/Makefile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running make with custom Makefile")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/dist/third-party/googletest/include)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/dist/third-party/googletest/lib)

enable_testing()

add_subdirectory(src)
if(ENABLE_GTEST)
  add_subdirectory(tests)
endif()
