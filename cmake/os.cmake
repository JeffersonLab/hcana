## OSX/homebrew version of root6 installs its cmake macros in a non-standard
## location. This might be an issue on other systems as well.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{ROOTSYS}/etc/root/cmake)
endif()

## Get rid of rpath warning on OSX
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_MACOSX_RPATH 1)
endif()
