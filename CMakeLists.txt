cmake_minimum_required(VERSION 3.12)

project(jaclks LANGUAGES C CXX)

# 全局标准设置
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_GTEST "Enable build googletest executable" ON)
option(ENABLE_COVERAGE "Enable build project with coverage options" OFF)

if(MSVC)
    # MSVC 编译器选项
    add_compile_options(/W4 /WX)  # /W4 警告级别，/WX 将警告视为错误
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    # GCC/Clang 编译器选项
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -pthread -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -pthread -fPIC")
endif()

if(ENABLE_GTEST AND ENABLE_COVERAGE)
    if(MSVC)
        message(WARNING "Code coverage is not properly supported with MSVC in this configuration")
    else()
        message(STATUS "Compile code with coverage option")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -fprofile-arcs -ftest-coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -fprofile-arcs -ftest-coverage")
        add_link_options(--coverage)
    endif()
endif()

message(STATUS "CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")

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
if(MSVC)
    add_custom_target(build_thirdparty
        COMMAND cmake -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest-build
        COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest-build &&
            cmake ../googletest
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/dist/third-party/googletest
            -DBUILD_SHARED_LIBS=OFF
            -Dgtest_force_shared_crt=ON
            -G "Ninja" &&
            cmake --build . --target install
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Building GTest with Ninja"
    )
else()
    add_custom_target(build_thirdparty
        COMMAND ${CMAKE_MAKE_PROGRAM} gtest -f ${CMAKE_CURRENT_SOURCE_DIR}/Makefile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running make with custom Makefile"
)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dist/third-party/googletest/include)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/dist/third-party/googletest/lib)

enable_testing()

add_subdirectory(src)
if(ENABLE_GTEST)
    add_subdirectory(tests)
endif()
