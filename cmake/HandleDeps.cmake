list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/dist/lib/cmake")

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

find_package(GTest REQUIRED)
if(GTEST_FOUND)
    message(STATUS "GTest version: ${GTEST_VERSION}")
    message(STATUS "GTest include dir: ${GTEST_INCLUDE_DIR}")
    message(STATUS "GTest libraries: ${GTEST_LIBRARIES}")
else()
    message(FATAL_ERROR "GTest not found")
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
