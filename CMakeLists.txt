cmake_minimum_required(VERSION 3.5)

project(hcana VERSION 0.90 LANGUAGES CXX)

#----------------------------------------------------------------------------
# Use our modules and utility functions
set(CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/podd/cmake/Modules
  ${CMAKE_MODULE_PATH}
  )
include(PoddCMakeEnv)

#----------------------------------------------------------------------------
# Install in GNU-style directory layout
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME_LC})

#----------------------------------------------------------------------------
# Find ROOT (using Podd's FindROOT.cmake)
find_package(ROOT 5.10 MODULE REQUIRED)

#----------------------------------------------------------------------------
# Set up the compiler flags
set_compiler_flags("${ROOT_CXX_FLAGS}")
set_diagnostic_flags(WALL WEXTRA)
report_build_info()

#----------------------------------------------------------------------------
# Directories in which we build things
add_subdirectory(podd)
add_subdirectory(src)

#----------------------------------------------------------------------------
# Install and export targets defined in the subdirectories
install(EXPORT ${PROJECT_NAME_LC}-exports
 FILE ${PROJECT_NAME}Targets.cmake
 NAMESPACE ${PROJECT_NAME}::
 DESTINATION ${INSTALL_CONFIGDIR}
 )
