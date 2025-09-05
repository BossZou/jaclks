set(SRC_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/jaclks-internal)

aux_source_directory(${SRC_ROOT_DIR}/javac-base/java/lang SRC_FILES)
aux_source_directory(${SRC_ROOT_DIR}/javac-base/java/security SRC_FILES)

add_library(jaclks_shared SHARED ${SRC_FILES})
if(PLATFORM_WINDOWS)
  target_compile_definitions(jaclks_shared PRIVATE MYDLL_EXPORTS)
endif()
target_include_directories(jaclks_shared PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(jaclks_shared PUBLIC MyThirdParty::All)
set_target_properties(
  jaclks_shared
  PROPERTIES OUTPUT_NAME "jaclks"
             ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
             RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

add_library(jaclks_static STATIC ${SRC_FILES})
if(PLATFORM_WINDOWS)
  target_compile_definitions(jaclks_static PRIVATE MYDLL_EXPORTS)
endif()
target_include_directories(jaclks_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(jaclks_static PUBLIC MyThirdParty::All)
set_target_properties(
  jaclks_static
  PROPERTIES OUTPUT_NAME "jaclks"
             ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
             RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Install library
include(GNUInstallDirs)

install(
  TARGETS jaclks_shared jaclks_static
  EXPORT JaclksTargets # cmake config (.cmake)
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # static lib (.a)
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # dynamic lib (.so)
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # Windows DLL
  INCLUDES
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # header files dir
)

install(DIRECTORY ${PROJECT_ROOT_DIR}/include/ DESTINATION include)

# Create Config file (optional, for find_package)
include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${PROJECT_ROOT_DIR}/Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/JaclksConfig.cmake
  INSTALL_DESTINATION lib/cmake/Jaclks)

# Install conf file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/JaclksConfig.cmake
        DESTINATION lib/cmake/Jaclks)

# Install export targets
install(
  EXPORT JaclksTargets
  FILE JaclksTargets.cmake
  NAMESPACE Jaclks::
  DESTINATION lib/cmake/Jaclks)
