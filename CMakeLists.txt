cmake_minimum_required(VERSION 3.5)

project(hcana VERSION 1.4 LANGUAGES CXX)

option(HCANA_BUILTIN_PODD "Use built-in Podd submodule (default: YES)" ON)

#----------------------------------------------------------------------------
# Set up Podd and ROOT dependencies
if(HCANA_BUILTIN_PODD)
  list(INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/podd/cmake/Modules")
  include(PoddCMakeEnv)
  find_package(ROOT 6.00 REQUIRED)
  config_add_dependency(ROOT 6.00)
else()
  # Find Podd and register it as a dependency
  # This will also automatically set up ROOT
  find_package(Podd 1.7 REQUIRED)
  include(PoddCMakeEnv)
  config_add_dependency(Podd 1.7)
endif()

#----------------------------------------------------------------------------
# Install in GNU-style directory layout
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/${MAIN_PROJECT_NAME_LC})
if(PODD_SET_RPATH)
  set_install_rpath()
endif()

#----------------------------------------------------------------------------
# Set up the compiler flags
set_compiler_flags("${ROOT_CXX_FLAGS}")
set_diagnostic_flags(WALL WEXTRA)
report_build_info()

#----------------------------------------------------------------------------
# Directories in which we build things
if(HCANA_BUILTIN_PODD)
  add_subdirectory(podd)
endif()
add_subdirectory(src)
add_subdirectory(cmake)

#----------------------------------------------------------------------------
# Optional features: JSON export and CCDB

include(CheckIncludeFileCXX)

option(HCANA_WITH_JSON "Enable JSON export of THcParmList (gHcParms)" ON)
option(HCANA_JSON_EXPORT_STRINGS
       "Export Textvars strings in JSON (requires newer Podd Textvars API)" OFF)
option(HCANA_WITH_CCDB "Enable CCDB support in THcParmList" OFF)

# If ROOT (or your system) exposes nlohmann/json.hpp via the include path,
# this check will succeed. ROOT 6.x vendors nlohmann::json internally, and
# many builds install <nlohmann/json.hpp> in the standard include path.

# Let the check see the same includes as normal code; ROOT_INCLUDE_DIRS is
# set by ROOT/Podd CMake.
if(DEFINED ROOT_INCLUDE_DIRS)
  set(CMAKE_REQUIRED_INCLUDES "${ROOT_INCLUDE_DIRS}")
endif()

if(HCANA_WITH_JSON)
  check_include_file_cxx("nlohmann/json.hpp" HCANA_HAVE_NLOHMANN_JSON)
  if(NOT HCANA_HAVE_NLOHMANN_JSON)
    message(WARNING
      "HCANA_WITH_JSON=ON but <nlohmann/json.hpp> not found. "
      "Disabling JSON export of THcParmList.")
    set(HCANA_WITH_JSON OFF)
  endif()
endif()

# These are the actual preprocessor symbols your code uses
if(HCANA_WITH_JSON)
  add_compile_definitions(WITH_JSON)
endif()

if(HCANA_WITH_CCDB)
  add_compile_definitions(WITH_CCDB)
endif()
