include_directories ("${PROJECT_SOURCE_DIR}/src/PluginApps/FrxPlugins")
SET(FRX_PLUGDK_SOURCES ${FRX_PLUGDK_SOURCES} frxPluginDK/FrxPluginWrapper.cpp
                                             frxPluginDK/FrxPlugMain.cpp)

IF (WIN32)
  ## FrxPlugins
  # TestPlugin
  add_library(frxTestPlugin SHARED TestPlugin/win/TestFrxPlug.cpp
                                   ${FRX_PLUGINDK_SOURCES})

  target_link_libraries (frxTestPlugin ${FRX_CLIBS})

ELSEIF(APPLE)
  ## FrxPlugins
  # TestPlugin
  ADD_EXECUTABLE(frxTestPlugin MACOSX_BUNDLE TestPlugin/mac/TestFrxPlug.cpp
                                   ${FRX_PLUGDK_SOURCES})
  
  SET_TARGET_PROPERTIES(frxTestPlugin PROPERTIES MACOSX_BUNDLE_INFO_PLIST "src/PluginApps/FrxPlugins/TestPlugin/mac/info.plist")
  SET_TARGET_PROPERTIES(frxTestPlugin PROPERTIES LINK_FLAGS "-bundle")

  target_link_libraries (frxTestPlugin ${FRX_CLIBS})

ENDIF(WIN32)
