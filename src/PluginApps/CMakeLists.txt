SET ( FRX_STANDALONE
        GuiStandalone/Main.cpp
)

add_executable(standalone ${FRX_STANDALONE})
target_link_libraries (standalone frx_core ${FRX_CLIBS})

IF(WIN32)
  SET (PlugSources ${PlugSources}
      VSTForx/win/VstForxResourceManager.cpp
      VSTForx/win/VstForxPlugMain.cpp
      VSTForx/win/initResourceMap.cpp
      ${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/vstplugmain.cpp
      VSTForx/win/vstplug.def
      VSTForx/win/resources.rc
   )
ELSEIF(APPLE)
  SET (PlugSources ${PlugSources} ${FRX_RESOURCES}
      VSTForx/mac/VstForxPlugMain.cpp
      VSTForx/mac/VstForxResourceManager.cpp
      VSTForx/mac/CocoaHelper.mm
      ${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/vstplugmain.cpp
   )
  SET_SOURCE_FILES_PROPERTIES(${FRX_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  set_source_files_properties(VSTForx/mac/CocoaHelper.mm PROPERTIES COMPILE_FLAGS "-x objective-c++")
ENDIF(WIN32)


IF (WIN32)
  add_library(vstforx SHARED ${PlugSources})
  target_link_libraries (vstforx frx_core ${FRX_CLIBS})

  add_library(vstforxInstrument SHARED ${PlugSources})
  target_link_libraries (vstforxInstrument frx_core ${FRX_CLIBS})
  target_compile_definitions(vstforxInstrument PUBLIC "FRX_IS_INSTRUMENT")

  add_library(vstforxDEMO SHARED ${PlugSources})
  target_link_libraries (vstforxDEMO frx_core ${FRX_CLIBS})
  target_compile_definitions(vstforxDEMO PUBLIC "FRX_IS_DEMO")

  add_library(vstforxInstrumentDEMO SHARED ${PlugSources})
  target_link_libraries (vstforxInstrumentDEMO frx_core ${FRX_CLIBS})
  target_compile_definitions(vstforxInstrumentDEMO PUBLIC "FRX_IS_INSTRUMENT" PUBLIC "FRX_IS_DEMO")


ELSEIF(APPLE)
  ADD_EXECUTABLE(vstforx MACOSX_BUNDLE ${PlugSources})
  target_link_libraries (vstforx frx_core ${FRX_CLIBS})
  SET_TARGET_PROPERTIES(vstforx PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/VSTForx/mac/vstforx.plist")
  SET_TARGET_PROPERTIES(vstforx PROPERTIES LINK_FLAGS "-bundle")
  target_compile_definitions(vstforx PUBLIC "FRX_BNDL_ID=\"com.sambagodschynski.frx.VSTForx\"")

  ADD_EXECUTABLE(vstforxInstrument MACOSX_BUNDLE ${PlugSources})
  target_link_libraries (vstforxInstrument frx_core ${FRX_CLIBS})
  SET_TARGET_PROPERTIES(vstforxInstrument PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/VSTForx/mac/ivstforx.plist")
  SET_TARGET_PROPERTIES(vstforxInstrument PROPERTIES LINK_FLAGS "-bundle")
  target_compile_definitions(vstforxInstrument PUBLIC "FRX_IS_INSTRUMENT" 
                                               PUBLIC "FRX_BNDL_ID=\"com.sambagodschynski.frx.VSTForxInstrument\"")

  ADD_EXECUTABLE(vstforxDEMO MACOSX_BUNDLE ${PlugSources})
  target_link_libraries (vstforxDEMO frx_core ${FRX_CLIBS})
  SET_TARGET_PROPERTIES(vstforxDEMO PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/VSTForx/mac/vstforxDemo.plist")
  SET_TARGET_PROPERTIES(vstforxDEMO PROPERTIES LINK_FLAGS "-bundle")
  target_compile_definitions(vstforxDEMO PUBLIC "FRX_IS_DEMO" 
                                         PUBLIC "FRX_BNDL_ID=\"com.sambagodschynski.frx.VSTForxDEMO\"")

  ADD_EXECUTABLE(vstforxInstrumentDEMO MACOSX_BUNDLE ${PlugSources})
  target_link_libraries (vstforxInstrumentDEMO frx_core ${FRX_CLIBS})
  SET_TARGET_PROPERTIES(vstforxInstrumentDEMO PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/VSTForx/mac/ivstforxDemo.plist")
  SET_TARGET_PROPERTIES(vstforxInstrumentDEMO PROPERTIES LINK_FLAGS "-bundle")
  target_compile_definitions(vstforxInstrumentDEMO PUBLIC "FRX_IS_INSTRUMENT"
                                                   PUBLIC "FRX_IS_DEMO"  
                                                   PUBLIC "FRX_BNDL_ID=\"com.sambagodschynski.frx.VSTForxInstrumentDEMO\"")

ENDIF(WIN32)

#currently no need for that
#add_subdirectory(PluginApps/FrxPlugins)


