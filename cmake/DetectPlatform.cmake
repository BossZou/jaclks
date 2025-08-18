# cmake/DetectPlatform.cmake

# Detect Operation System
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM_WINDOWS ON CACHE INTERNAL "Targeting Windows platform")
    set(PLATFORM_NAME "windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM_LINUX ON CACHE INTERNAL "Targeting Linux platform")
    set(PLATFORM_NAME "linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(PLATFORM_MACOS ON CACHE INTERNAL "Targeting macOS platform")
    set(PLATFORM_NAME "macos")
else()
    message(WARNING "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
    set(PLATFORM_NAME "unknown")
endif()

# Detect CPU Architecture
#if(CMAKE_SIZEOF_VOID_P EQUAL 8)
#    set(ARCH_64BIT ON CACHE INTERNAL "64-bit architecture")
#    set(ARCH_NAME "x64")
#else()
#    set(ARCH_32BIT ON CACHE INTERNAL "32-bit architecture")
#    set(ARCH_NAME "x86")
#endif()

# Detect CPU Architecture
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" ARCH_DETAILED)
if(ARCH_DETAILED MATCHES "amd64|x86_64")
    set(ARCH_NAME "x64")
elseif(ARCH_DETAILED MATCHES "i[3-6]86")
    set(ARCH_NAME "x86")
elseif(ARCH_DETAILED MATCHES "armv7")
    set(ARCH_NAME "arm32")
elseif(ARCH_DETAILED MATCHES "aarch64|arm64")
    set(ARCH_NAME "arm64")
else()
    set(ARCH_NAME ${ARCH_DETAILED})
endif()

# Combine platform and architecture
set(PLATFORM_ARCH "${PLATFORM_NAME}-${ARCH_NAME}" CACHE STRING "Platform-architecture identifier")

# Print results
message(STATUS "Detected platform: ${PLATFORM_NAME}")
message(STATUS "Detected architecture: ${ARCH_NAME}")
message(STATUS "Platform-arch identifier: ${PLATFORM_ARCH}")
