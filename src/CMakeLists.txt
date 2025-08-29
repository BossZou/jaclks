aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/jaclks-internal/javac-base/lang
                     SRC_FILES)
aux_source_directory(
  ${CMAKE_CURRENT_SOURCE_DIR}/jaclks-internal/javac-base/java/security
  SRC_FILES)

add_library(jaclks SHARED ${SRC_FILES})
target_include_directories(jaclks PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Install library
include(GNUInstallDirs)

install(
  TARGETS jaclks
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
