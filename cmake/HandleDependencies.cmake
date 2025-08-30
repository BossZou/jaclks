# cmake/HandleDependencies.cmake

# Initial and update git submodule message(STATUS "HandleDependencies
# Dir=${PROJECT_SOURCE_DIR}") execute_process( COMMAND ${GIT_EXECUTABLE}
# submodule update --init --recursive WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
# RESULT_VARIABLE GIT_SUBMOD_RESULT)
#
# if(NOT GIT_SUBMOD_RESULT EQUAL "0") message( FATAL_ERROR "Failed to initialize
# git submodules: ${GIT_SUBMOD_RESULT}") endif()

# Build submodule projects
set(THIRD_PARTY_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)
set(THIRD_PARTY_DIST_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dist)

message(
  STATUS
    "Build googletest on dir ${THIRD_PARTY_ROOT_DIR}/googletest, build dir ${THIRD_PARTY_ROOT_DIR}/googletest/build"
)
execute_process(
  COMMAND
    cmake -B ${THIRD_PARTY_ROOT_DIR}/googletest/build -S
    ${THIRD_PARTY_ROOT_DIR}/googletest -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_DIST_ROOT_DIR}/googletest
    -DBUILD_SHARED_LIBS=OFF -Dgtest_force_shared_crt=ON
    -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_DIST_ROOT_DIR} -DENABLE_GTEST=OFF
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  RESULT_VARIABLE SUBMOD_BUILD_RESULT)
if(NOT SUBMOD_BUILD_RESULT EQUAL "0")
  message(FATAL_ERROR "Failed to build git submodules: ${GIT_SUBMOD_RESULT}")
else()
  message(STATUS "Build googletest: ${GIT_SUBMOD_RESULT}")
endif()

# unified all third-party dependencies
include(CMakeFindDependencyMacro)

# Git required to update third-party
find_package(Git REQUIRED)

find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
  message(STATUS "OpenSSL version: ${OPENSSL_VERSION}")
  message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
  message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
else()
  message(FATAL_ERROR "OpenSSL not found")
endif()

set(MyThirdParty_FOUND TRUE)

if(NOT TARGET MyThirdParty::All)
  add_library(MyThirdParty::All INTERFACE IMPORTED)

  # 添加所有依赖
  target_link_libraries(MyThirdParty::All INTERFACE OpenSSL::SSL
                                                    OpenSSL::Crypto)

  # # 添加编译定义（如果需要） target_compile_definitions(MyThirdParty::All INTERFACE
  # USE_OPENSSL=1 USE_BOOST=1 )
endif()
