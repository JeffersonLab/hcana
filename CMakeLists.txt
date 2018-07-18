cmake_minimum_required(VERSION 3.5)

project(hcana VERSION 0.90 LANGUAGES CXX)

option(HCANA_BUILTIN_PODD "Use built-in Podd submodule (default: YES)" ON)

#----------------------------------------------------------------------------
# Use Podd modules and utility functions
if(HCANA_BUILTIN_PODD)
  set(CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/podd/cmake/Modules
    ${CMAKE_MODULE_PATH}
    )
  include(PoddCMakeEnv)
  find_package(ROOT 5.10 REQUIRED)
else()
  # This will automatically set up ROOT
  find_package(Podd 1.6 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Set up the compiler flags
set_compiler_flags("${ROOT_CXX_FLAGS}")
set_diagnostic_flags(WALL WEXTRA)
report_build_info()

#----------------------------------------------------------------------------
# Install in GNU-style directory layout
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME_LC})

#----------------------------------------------------------------------------
# Directories in which we build things
if(HCANA_BUILTIN_PODD)
  add_subdirectory(podd)
endif()
add_subdirectory(src)

#----------------------------------------------------------------------------
# Install and export targets defined in the subdirectories
install(EXPORT ${PROJECT_NAME_LC}-exports
 FILE ${PROJECT_NAME}Targets.cmake
 NAMESPACE ${PROJECT_NAME}::
 DESTINATION ${INSTALL_CONFIGDIR}
 )
