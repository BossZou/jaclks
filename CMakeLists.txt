cmake_minimum_required(VERSION 3.12)

project(jaclks LANGUAGES C CXX)

# 全局标准设置
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -pthread -fPIC")

option(ENABLE_GTEST "Enable build googletest executable" ON)
option(ENABLE_COVERAGE "Enable build project with coverage options" OFF)

if(ENABLE_GTEST)
    if(ENABLE_COVERAGE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    endif()
endif()

find_package(Git REQUIRED)

# 初始化并更新子模块
execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMOD_RESULT
)

if(NOT GIT_SUBMOD_RESULT EQUAL "0")
    message(FATAL_ERROR "Failed to initialize git submodules: ${GIT_SUBMOD_RESULT}")
endif()

# Third party configurations
add_custom_target(build_thirdparty
        COMMAND ${CMAKE_MAKE_PROGRAM} gtest -f ${CMAKE_CURRENT_SOURCE_DIR}/Makefile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running make with custom Makefile"
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dist/third-party/googletest/include)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/dist/third-party/googletest/lib)

enable_testing()

add_subdirectory(src)
if(ENABLE_GTEST)
    add_subdirectory(tests)
endif()

