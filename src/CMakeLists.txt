cmake_minimum_required(VERSION 3.5)

set(HCANA_src 
Scaler9001.cxx Scaler9250.cxx THcAerogel.cxx THcAerogelHit.cxx THcAnalyzer.cxx THcBCMCurrent.cxx THcCherenkov.cxx
THcCherenkovHit.cxx THcCoinTime.cxx THcConfigEvtHandler.cxx THcDC.cxx THcDCHit.cxx THcDCLookupTTDConv.cxx THcDCTimeToDistConv.cxx
THcDCTrack.cxx THcDCWire.cxx THcDetectorMap.cxx THcDriftChamber.cxx THcDriftChamberPlane.cxx THcDummySpectrometer.cxx THcExtTarCor.cxx
THcFormula.cxx THcHallCSpectrometer.cxx THcHitList.cxx THcHodoEff.cxx THcHodoHit.cxx THcHodoscope.cxx THcInterface.cxx
THcParmList.cxx THcPeriodicReport.cxx THcPrimaryKine.cxx THcRaster.cxx THcRasteredBeam.cxx THcRasterRawHit.cxx THcRawAdcHit.cxx
THcRawDCHit.cxx THcRawHit.cxx THcRawHodoHit.cxx THcRawShowerHit.cxx THcRawTdcHit.cxx THcReactionPoint.cxx THcRun.cxx
THcRunParameters.cxx THcScalerEvtHandler.cxx THcScintillatorPlane.cxx THcSecondaryKine.cxx THcShowerArray.cxx THcShower.cxx THcShowerHit.cxx
THcShowerPlane.cxx THcSignalHit.cxx THcSpacePoint.cxx THcTimeSyncEvtHandler.cxx THcTrigApp.cxx THcTrigDet.cxx THcTrigRawHit.cxx TIBlobModule.cxx
  )

# This would be alot easier if there was an "include" directory
set(headers
  Scaler9001.h Scaler9250.h THcAerogel.h THcAerogelHit.h THcAnalyzer.h THcBCMCurrent.h THcCherenkov.h THcCherenkovHit.h THcCoinTime.h THcConfigEvtHandler.h
  THcDC.h THcDCHit.h THcDCLookupTTDConv.h THcDCTimeToDistConv.h THcDCTrack.h THcDCWire.h THcDetectorMap.h
  THcDriftChamber.h THcDriftChamberPlane.h THcDummySpectrometer.h THcExtTarCor.h THcFormula.h THcGlobals.h THcHallCSpectrometer.h
  THcHitList.h THcHodoEff.h THcHodoHit.h THcHodoscope.h THcInterface.h THcParmList.h THcPeriodicReport.h
  THcPrimaryKine.h THcRasteredBeam.h THcRaster.h THcRasterRawHit.h THcRawAdcHit.h THcRawDCHit.h THcRawHit.h
  THcRawHodoHit.h THcRawShowerHit.h THcRawTdcHit.h THcReactionPoint.h THcRun.h THcRunParameters.h THcScalerEvtHandler.h THcScintillatorPlane.h
  THcSecondaryKine.h THcShowerArray.h THcShower.h THcShowerHit.h THcShowerPlane.h THcSignalHit.h THcSpacePoint.h THcTimeSyncEvtHandler.h
  THcTrigApp.h THcTrigDet.h THcTrigRawHit.h TIBlobModule.h
  )


ROOT_GENERATE_DICTIONARY(HallCDict ${headers} 
  INCLUDEDIRS include ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include
  LINKDEF include/HallC_LinkDef.h  
  OPTIONS -p)
set(PCM_FILE  ${CMAKE_CURRENT_BINARY_DIR}/HallCDict_rdict.pcm)

#SET(lib_HEADERS ${lib_HEADERS})
#SET(lib_SRCS ${lib_Fortran_SRCs} ${lib_SRCS} ${lib_DICTIONARY_SRC})
#SET(lib_VERSION "${${PROJECT_NAME}_VERSION}")
#SET(lib_MAJOR_VERSION "${${PROJECT_NAME}_MAJOR_VERSION}")
#SET(lib_LIBRARY_PROPERTIES 
#    VERSION   "${lib_VERSION}"
#    SOVERSION "${lib_MAJOR_VERSION}"
#    SUFFIX ".so")

ADD_CUSTOM_TARGET(HallC_ROOTDICTS DEPENDS ${HCANA_src} ${headers} include/HallC_LinkDef.h HallCDict.cxx)
add_library(HallC SHARED
  ${HCANA_src} HallCDict.cxx
  )

add_library(HCANA::HallC ALIAS HallC)

#Set target properties
target_include_directories(HallC
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

#target_compile_features(HallC PRIVATE cxx_auto_type)
target_compile_options(HallC PRIVATE $<$<CXX_COMPILER_ID:GNU>:-DHAS_SSTREAM=1>)

target_link_libraries(HallC
  #PUBLIC ${EXPAT_LIBRARIES}
  #PUBLIC ${CMAKE_THREAD_LIBS_INIT}
  PUBLIC ${ROOT_LIBRARIES} 
  PUBLIC PODD::HallA
  PUBLIC PODD::Decode
)
#This is required so that the exported target has the name JSONUtils and not jsonutils
#set_target_properties(HallC PROPERTIES EXPORT_NAME hallC)

add_executable(hc_analyzer main.C)
target_include_directories(hc_analyzer
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

#target_compile_features(HallC PRIVATE cxx_auto_type)
target_compile_options(hc_analyzer PRIVATE $<$<CXX_COMPILER_ID:GNU>:-DHAS_SSTREAM=1>)
target_link_libraries(hc_analyzer
  PUBLIC HallC
  PUBLIC PODD::HallA
  PUBLIC PODD::Decode
  PUBLIC ${ROOT_LIBRARIES} 
)

install(TARGETS HallC hc_analyzer
  EXPORT HCANA-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )


install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hcana)
install(FILES ${PCM_FILE} DESTINATION lib)

#Export the targets to a script
install(EXPORT HCANA-targets
  FILE
  HCANATargets.cmake
  NAMESPACE
  HCANA::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)
