cmake_minimum_required(VERSION 3.5)

project(hcana VERSION 0.90 LANGUAGES CXX)

option(HCANA_BUILTIN_PODD "Use built-in Podd submodule (default: YES)" ON)

#----------------------------------------------------------------------------
# Set up Podd and ROOT dependencies
if(HCANA_BUILTIN_PODD)
  set(CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/podd/cmake/Modules
    ${CMAKE_MODULE_PATH}
    )
  include(PoddCMakeEnv)
  find_package(ROOT 5.10 REQUIRED)
else()
  # Find Podd and register it as a dependency
  # This will also automatically set up ROOT
  find_package(Podd 1.6 REQUIRED)
  include(PoddCMakeEnv)
  config_add_dependency(Podd 1.6)
endif()

# Register ROOT dependency - it's in our public interface
config_add_dependency(ROOT 5.10)

#----------------------------------------------------------------------------
# Set up the compiler flags
set_compiler_flags("${ROOT_CXX_FLAGS}")
set_diagnostic_flags(WALL WEXTRA)
report_build_info()

#----------------------------------------------------------------------------
# Install in GNU-style directory layout
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/${MAIN_PROJECT_NAME_LC})

#----------------------------------------------------------------------------
# Directories in which we build things
if(HCANA_BUILTIN_PODD)
  add_subdirectory(podd)
endif()
add_subdirectory(src)
add_subdirectory(cmake)
