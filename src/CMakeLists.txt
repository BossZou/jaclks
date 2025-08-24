aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/concurrent SRC_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/javac-base/lang SRC_FILES)

add_library(jaclks SHARED ${SRC_FILES})

# Install library
include(GNUInstallDirs)

install(TARGETS jaclks
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}      # static lib (.a)
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}      # dynamic lib (.so)
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}      # Windows DLL
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # header files dir
)

install(DIRECTORY ${PROJECT_ROOT_DIR}/include/ DESTINATION include)
