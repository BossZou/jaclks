list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/dist/lib/cmake")

# unified all third-party dependencies
include(CMakeFindDependencyMacro)

# Git required to update third-party
find_package(Git REQUIRED)

find_package(OpenSSL)
if(OPENSSL_FOUND)
  message(STATUS "OpenSSL version: ${OPENSSL_VERSION}")
  message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
  message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
else()
  message(WARNING "OpenSSL find failed. OPENSSL_INCLUDE_DIR=${OPENSSL_INCLUDE_DIR}")
  message(WARNING "OpenSSL find failed. OPENSSL_CRYPTO_LIBRARY=${OPENSSL_CRYPTO_LIBRARY}")
  message(WARNING "OpenSSL find failed. OPENSSL_SSL_LIBRARY=${OPENSSL_SSL_LIBRARY}")
  message(WARNING "OpenSSL find failed. OPENSSL_LIBRARIES=${OPENSSL_LIBRARIES}")
  message(WARNING "OpenSSL find failed. OPENSSL_VERSION=${OPENSSL_VERSION}")
  message(FATAL_ERROR "OpenSSL not found")
endif()

if(ENABLE_GTEST)
  find_package(GTest REQUIRED)
  if(GTest_FOUND)
    message(STATUS "GTest found: ${GTest_FOUND}")
    message(STATUS "GTest version: ${GTest_VERSION}")

    message(
      STATUS
        "GTest targets available: GTest::gtest, GTest::gtest_main, GTest::gmock, GTest::gmock_main"
    )

    if(DEFINED GTEST_INCLUDE_DIRS)
      message(STATUS "GTest include dirs: ${GTEST_INCLUDE_DIRS}")
    endif()

    if(DEFINED GTEST_LIBRARIES)
      message(STATUS "GTest libraries: ${GTEST_LIBRARIES}")
    endif()
  else()
    message(FATAL_ERROR "GTest not found")
  endif()
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
