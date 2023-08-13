cmake_minimum_required(VERSION 3.5)

#----------------------------------------------------------------------------
# Names of the main items we build here
set(LIBNAME HallC)
set(EXENAME hcana)

#----------------------------------------------------------------------------
# Sources and headers
file(GLOB src "*.cxx")
file(GLOB allheaders "*.h")
list(REMOVE_ITEM allheaders
  "${CMAKE_CURRENT_SOURCE_DIR}/${LIBNAME}_LinkDef.h"
  )
set(headers "${allheaders}")
list(REMOVE_ITEM headers
  "${CMAKE_CURRENT_SOURCE_DIR}/hc_compiledata.h"
  )
set(classheaders "${headers}")
list(REMOVE_ITEM classheaders
  "${CMAKE_CURRENT_SOURCE_DIR}/THcGlobals.h"
  )

#----------------------------------------------------------------------------
# Generate HallC_LinkDef.h
set(linkdef "${CMAKE_CURRENT_BINARY_DIR}/${LIBNAME}_LinkDef.h")
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/${LIBNAME}_LinkDef.h_preamble" preamble)
file(WRITE "${linkdef}" "${preamble}")
unset(preamble)
foreach(hdr IN LISTS classheaders)
  get_filename_component(basename "${hdr}" NAME_WE)
  if("${basename}" MATCHES "^Scaler" OR "${basename}" STREQUAL "TIBlobModule")
    file(APPEND ${linkdef} "#pragma link C++ class Decoder::${basename}+;\n")
  else()
    file(APPEND ${linkdef} "#pragma link C++ class ${basename}+;\n")
  endif()
endforeach()
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/${LIBNAME}_LinkDef.h_postamble" postamble)
file(APPEND "${linkdef}" "${postamble}")
unset(postamble)

#----------------------------------------------------------------------------
# Generate hc_compiledata.h
include(PoddCompileInfo)

# Ensure there's no old hc_compiledata.h hanging around from a non-CMake build
file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/hc_compiledata.h")

configure_file(hc_compiledata.h.in hc_compiledata.h)
list(APPEND allheaders "${CMAKE_CURRENT_BINARY_DIR}/hc_compiledata.h")

#----------------------------------------------------------------------------
# libHallC
add_library(${LIBNAME} SHARED ${src} ${allheaders} ${LIBNAME}Dict.cxx)
add_library(${PROJECT_NAME}::${LIBNAME} ALIAS ${LIBNAME})

target_include_directories(${LIBNAME}
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>  # for hc_compiledata.h
  )

target_compile_options(${LIBNAME}
  PUBLIC
    ${${PROJECT_NAME_UC}_CXX_FLAGS_LIST}
  PRIVATE
    ${${PROJECT_NAME_UC}_DIAG_FLAGS_LIST}
  )
if(WITH_DEBUG)
  target_compile_definitions(${LIBNAME} PUBLIC WITH_DEBUG)
endif()

target_link_libraries(${LIBNAME}
  PUBLIC
    Podd::Podd
    Podd::Decode
  )
set_target_properties(${LIBNAME} PROPERTIES
  SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  VERSION ${PROJECT_VERSION}
  EXPORT_NAME ${LIBNAME}
  )
add_dependencies(${LIBNAME} gitrev_${PROJECT_NAME})

install(TARGETS ${LIBNAME}
  EXPORT ${MAIN_PROJECT_NAME_LC}-exports
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
install(FILES ${allheaders} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#----------------------------------------------------------------------------
# libHallCDict
build_root_dictionary(${LIBNAME} ${headers}
  TARGETS ${LIBNAME}
  LINKDEF ${linkdef}
  )

#----------------------------------------------------------------------------
# hcana executable
add_executable(${EXENAME} main.C)

target_link_libraries(${EXENAME}
  PRIVATE
    ${LIBNAME}
  )
target_compile_options(${EXENAME}
  PUBLIC
    ${${PROJECT_NAME_UC}_CXX_FLAGS_LIST}
  PRIVATE
    ${${PROJECT_NAME_UC}_DIAG_FLAGS_LIST}
  )
if(${CMAKE_SYSTEM_NAME} MATCHES Linux)
  # Linux (at least with g++) requires -fPIC even for main
  target_compile_options(${EXENAME} PUBLIC -fPIC)
endif()

install(TARGETS ${EXENAME}
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
