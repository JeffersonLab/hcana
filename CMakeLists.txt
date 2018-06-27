cmake_minimum_required(VERSION 3.5)
project(HCANA VERSION 1.6.0 LANGUAGES C CXX)

set(CMAKE_MODULE_PATH
   ${CMAKE_MODULE_PATH}
   ${PROJECT_SOURCE_DIR}/cmake
   ${PROJECT_SOURCE_DIR}/cmake/modules)

## Use C++17 features 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## OS Specific settings
include(cmake/os.cmake)
# provides variables for header hc_compiledata.h
include(cmake/modules/CompileTimeInfo.cmake)

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR lib/hcana)

find_package(EXPAT REQUIRED COMPONENTS EXPAT )
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

find_package(EVIO REQUIRED)
find_package(PODD REQUIRED)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS}/etc/cmake)
find_package(ROOT REQUIRED COMPONENTS Tree)
include(${ROOT_USE_FILE})
# extra root settings
include_directories(${ROOT_INCLUDE_DIRS})
link_directories(${ROOT_LIBRARY_DIR})
include(cmake/root.cmake)

# this helps with the circular headers
include_directories(src)

# because the current includes don't use "podd/x.h"
#message("PODD_INCLUDE_DIR ${PODD_INCLUDE_DIR}")
include_directories(${PODD_INCLUDE_DIR}/podd)


configure_file(hc_compiledata.h.in 
  src/hc_compiledata.h)

#add_subdirectory(hana_decode)
add_subdirectory(src)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/src/hc_compiledata.h
  DESTINATION include/hcana
  )
##############################################
# Installation


#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/HCANAConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
  )

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/HCANAConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/HCANAConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
  #    ${CMAKE_CURRENT_LIST_DIR}/cmake/FindRapidJSON.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/HCANAConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/HCANAConfigVersion.cmake
  DESTINATION ${INSTALL_CONFIGDIR}
)

##############################################
## Exporting from the build tree
#configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/FindRapidJSON.cmake
#    ${CMAKE_CURRENT_BINARY_DIR}/FindRapidJSON.cmake
#    COPYONLY)

export(EXPORT HCANA-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/HCANATargets.cmake NAMESPACE HCANA::)

#Register package in user's package registry
export(PACKAGE HCANA) 
