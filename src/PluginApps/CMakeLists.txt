SET ( FRX_STANDALONE
        GuiStandalone/Main.cpp
	com/FrxPlugSettings.cpp
	VSTForx/win/resources.rc)

# common files
SET (PlugSources ${PlugSources}
    com/FrxPlugSettings.cpp
)


#remote channel common sources
SET (RemoteSrc ${RemoteSrc} RemoteChannel/Plugin.cpp)


#bridge
add_executable(VSTForx.bridge Bridge/main.cpp
		      com/FrxPlugSettings.cpp	
)
target_link_libraries (VSTForx.bridge frx_core ${FRX_CLIBS})

IF(WIN32)
  #standalone
  add_executable(standalone ${FRX_STANDALONE})
  target_link_libraries (standalone frx_core ${FRX_CLIBS})

  #vstforx
  SET (PlugSources ${PlugSources}
      VSTForx/win/VstForxResourceManager.cpp
      VSTForx/win/VstForxPlugMain.cpp
      VSTForx/win/initResourceMap.cpp
      ${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/vstplugmain.cpp
      VSTForx/win/vstplug.def
      VSTForx/win/resources.rc
   )
  #remote channel
  SET (RemoteSrc ${RemoteSrc}
  	RemoteChannel/win/RemoteChannelMain.cpp
   	${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/vstplugmain.cpp
     	RemoteChannel/win/vstplug.def
   )

ELSEIF(APPLE)
  #vstforx
  SET (PlugSources ${PlugSources} ${FRX_RESOURCES}
      VSTForx/mac/VstForxPlugMain.cpp
      VSTForx/mac/VstForxResourceManager.cpp
      com/mac/CocoaHelper.mm
      ${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/vstplugmain.cpp
   )
  SET_SOURCE_FILES_PROPERTIES(${FRX_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  set_source_files_properties(VSTForx/mac/CocoaHelper.mm PROPERTIES COMPILE_FLAGS "-x objective-c++")

  #CM-Version
  SET(FRX_CM_RESOURCES 
      ${FRX_CM_RESOURCES} 
      ${FRX_RESOURCES}
      ${PROJECT_SOURCE_DIR}/src/images/cmlogo.png)
  SET_SOURCE_FILES_PROPERTIES(${FRX_CM_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  SET (CMPlugSources ${CMPlugSources} ${PlugSources} ${FRX_CM_RESOURCES})

  #remote channel
  SET (RemoteSrc ${RemoteSrc}
        RemoteChannel/mac/RemoteChannelMain.cpp
        com/mac/CocoaHelper.mm
	${VSTSDK_INCLUDE_DIRS}/public.sdk/source/vst2.x/vstplugmain.cpp
   )
ENDIF(WIN32)

IF (WIN32)
  #vstforx
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

#  add_library(VSTForx-CM SHARED ${PlugSources}) 
#  target_link_libraries (VSTForx-CM frx_core ${FRX_CLIBS})
#  target_compile_definitions(VSTForx-CM PUBLIC "FRX_IS_CM")

#  add_library(VSTForxInstrument-CM SHARED ${PlugSources}) 
#  target_link_libraries (VSTForxInstrument-CM frx_core ${FRX_CLIBS})
#  target_compile_definitions(VSTForxInstrument-CM PUBLIC "FRX_IS_INSTRUMENT" PUBLIC "FRX_IS_CM")

  #remote channel
  add_library(remoteChannelSender SHARED ${RemoteSrc})
  target_link_libraries (remoteChannelSender frx_core ${FRX_CLIBS})

  add_library(remoteChannelSender_DEMO SHARED ${RemoteSrc})
  target_link_libraries (remoteChannelSender_DEMO frx_core ${FRX_CLIBS})
  target_compile_definitions(remoteChannelSender_DEMO PUBLIC "FRX_REMOTE_IS_DEMO")

#process plistfiles
ELSEIF(APPLE)
	#standalone
	SET(PLIST_VERSION "${FRX_VERSION_MAJOR}.${FRX_VERSION_MINOR}.${FRX_VERSION_MICRO}")
	SET(PLIST_PROD Standalone)
	SET(PLIST_EXEC standalone)
	CONFIGURE_FILE(
  	  "tmpl.plist.in"
  	  "VSTForx/mac/standalone.plist"
	)	
	#vstforx
	SET(PLIST_VERSION "${FRX_VERSION_MAJOR}.${FRX_VERSION_MINOR}.${FRX_VERSION_MICRO}")
	SET(PLIST_PROD VSTForx)
	SET(PLIST_EXEC vstforx)
	CONFIGURE_FILE(
  	  "tmpl.plist.in"
  	  "VSTForx/mac/vstforx.plist"
	)	
	SET(PLIST_PROD VSTForxInstrument)
	SET(PLIST_EXEC vstforxInstrument)
	CONFIGURE_FILE(
  	  "tmpl.plist.in"
  	  "VSTForx/mac/ivstforx.plist"
	)
	SET(PLIST_PROD VSTForx-CM)
	SET(PLIST_EXEC VSTForx-CM)
	CONFIGURE_FILE(
  	  "tmpl.plist.in"
  	  "VSTForx/mac/vstforxcm.plist"
	)	
	SET(PLIST_PROD VSTForxInstrument-CM)
	SET(PLIST_EXEC VSTForxInstrument-CM)
	CONFIGURE_FILE(
  	  "tmpl.plist.in"
  	  "VSTForx/mac/ivstforxcm.plist"
	)		
	SET(PLIST_PROD VSTForxDEMO)
	SET(PLIST_EXEC vstforxDEMO)
	CONFIGURE_FILE(
  	  "tmpl.plist.in"
  	  "VSTForx/mac/vstforxDemo.plist"
	)	
	SET(PLIST_PROD VSTForxInstrumentDEMO)
	SET(PLIST_EXEC vstforxInstrumentDEMO)
	CONFIGURE_FILE(
  	  "tmpl.plist.in"
  	  "ivstforxDEMO.plist"
	)
	#remote channel
	SET(PLIST_PROD RemoteChannelSender)
	SET(PLIST_EXEC remoteChannelSender)
	CONFIGURE_FILE(
  	  "tmpl.plist.in"
  	  "RemoteChannel/mac/remoteChannel.plist"
	)		
	SET(PLIST_PROD RemoteChannelSender_DEMO)
	SET(PLIST_EXEC remoteChannelSender_DEMO)
	CONFIGURE_FILE(
  	  "tmpl.plist.in"
  	  "RemoteChannel/mac/remoteChannel_DEMO.plist"
	)		
  #set executables
  #standalone
  add_executable(standalone MACOSX_BUNDLE ${FRX_STANDALONE})
  target_link_libraries (standalone frx_core ${FRX_CLIBS})
  SET_TARGET_PROPERTIES(standalone PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/VSTForx/mac/standalone.plist")
  SET_TARGET_PROPERTIES(standalone PROPERTIES LINK_FLAGS "-bundle")
  target_compile_definitions(standalone PUBLIC "FRX_BNDL_ID=\"com.sambagodschynski.frx.Standalone\"")


  #vstforx
  ADD_EXECUTABLE(vstforx MACOSX_BUNDLE ${PlugSources})
  target_link_libraries (vstforx frx_core ${FRX_CLIBS})
  SET_TARGET_PROPERTIES(vstforx PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/VSTForx/mac/vstforx.plist")
  SET_TARGET_PROPERTIES(vstforx PROPERTIES LINK_FLAGS "-bundle")
  target_compile_definitions(vstforx PUBLIC "FRX_BNDL_ID=\"com.sambagodschynski.frx.VSTForx\"")
  
  #vstforxInstrument
  ADD_EXECUTABLE(vstforxInstrument MACOSX_BUNDLE ${PlugSources})
  target_link_libraries (vstforxInstrument frx_core ${FRX_CLIBS})
  SET_TARGET_PROPERTIES(vstforxInstrument PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/VSTForx/mac/ivstforx.plist")
  SET_TARGET_PROPERTIES(vstforxInstrument PROPERTIES LINK_FLAGS "-bundle")
  target_compile_definitions(vstforxInstrument PUBLIC "FRX_IS_INSTRUMENT"
  					       PUBLIC "FRX_BNDL_ID=\"com.sambagodschynski.frx.VSTForxInstrument\"")
  #vstforx-CM
#  ADD_EXECUTABLE(VSTForx-CM MACOSX_BUNDLE ${CMPlugSources} )
#  target_link_libraries (VSTForx-CM frx_core ${FRX_CLIBS})
#  SET_TARGET_PROPERTIES(VSTForx-CM PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/VSTForx/mac/vstforxcm.plist")
#  SET_TARGET_PROPERTIES(VSTForx-CM PROPERTIES LINK_FLAGS "-bundle")
#  target_compile_definitions(VSTForx-CM PUBLIC "FRX_IS_CM"
#                                        PUBLIC "FRX_BNDL_ID=\"com.sambagodschynski.frx.VSTForx-CM\"")

  #vstforxInstrument-CM
#  ADD_EXECUTABLE(VSTForxInstrument-CM MACOSX_BUNDLE ${CMPlugSources})
#  target_link_libraries (VSTForxInstrument-CM frx_core ${FRX_CLIBS})
#  SET_TARGET_PROPERTIES(VSTForxInstrument-CM PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/VSTForx/mac/ivstforxcm.plist")
#  SET_TARGET_PROPERTIES(VSTForxInstrument-CM PROPERTIES LINK_FLAGS "-bundle")
#  target_compile_definitions(VSTForxInstrument-CM PUBLIC "FRX_IS_INSTRUMENT"
#  						  PUBLIC "FRX_IS_CM"
#  					          PUBLIC "FRX_BNDL_ID=\"com.sambagodschynski.frx.VSTForxInstrument-CM\"")
 
  #vstforxDemo
  ADD_EXECUTABLE(vstforxDEMO MACOSX_BUNDLE ${PlugSources})
  target_link_libraries (vstforxDEMO frx_core ${FRX_CLIBS})
  SET_TARGET_PROPERTIES(vstforxDEMO PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/VSTForx/mac/vstforxDemo.plist")
  SET_TARGET_PROPERTIES(vstforxDEMO PROPERTIES LINK_FLAGS "-bundle")
  target_compile_definitions(vstforxDEMO PUBLIC "FRX_IS_DEMO"
  					 PUBLIC "FRX_BNDL_ID=\"com.sambagodschynski.frx.VSTForxDEMO\"")
  #vstforxDemoInstrument
  ADD_EXECUTABLE(vstforxInstrumentDEMO MACOSX_BUNDLE ${PlugSources})
  target_link_libraries (vstforxInstrumentDEMO frx_core ${FRX_CLIBS})
  SET_TARGET_PROPERTIES(vstforxInstrumentDEMO PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/VSTForx/mac/ivstforxDemo.plist")
  SET_TARGET_PROPERTIES(vstforxInstrumentDEMO PROPERTIES LINK_FLAGS "-bundle")
  target_compile_definitions(vstforxInstrumentDEMO PUBLIC "FRX_IS_INSTRUMENT"
                                                   PUBLIC "FRX_IS_DEMO"
						   PUBLIC "FRX_BNDL_ID=\"com.sambagodschynski.frx.VSTForxInstrumentDEMO\"")

  #remote channel
  ADD_EXECUTABLE(remoteChannelSender MACOSX_BUNDLE ${RemoteSrc})
  target_link_libraries (remoteChannelSender frx_core ${FRX_CLIBS})
  SET_TARGET_PROPERTIES(remoteChannelSender PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/RemoteChannel/mac/remoteChannel.plist")
  SET_TARGET_PROPERTIES(remoteChannelSender PROPERTIES LINK_FLAGS "-bundle")
  target_compile_definitions(remoteChannelSender PUBLIC "FRX_BNDL_ID=\"com.sambagodschynski.frx.RemoteChannelSender\"")	

  ADD_EXECUTABLE(remoteChannelSender_DEMO MACOSX_BUNDLE ${RemoteSrc})
  target_link_libraries (remoteChannelSender_DEMO frx_core ${FRX_CLIBS})
  SET_TARGET_PROPERTIES(remoteChannelSender_DEMO PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/RemoteChannel/mac/remoteChannel_DEMO.plist")
  SET_TARGET_PROPERTIES(remoteChannelSender_DEMO PROPERTIES LINK_FLAGS "-bundle")
  target_compile_definitions(remoteChannelSender_DEMO PUBLIC "FRX_BNDL_ID=\"com.sambagodschynski.frx.RemoteChannelSenderDEMO\""
												 PUBLIC "FRX_REMOTE_IS_DEMO")

ENDIF(WIN32)

#currently no need for that
#add_subdirectory(PluginApps/FrxPlugins)


