cmake_minimum_required(VERSION 3.15)

# ── Core sources ──────────────────────────────────────────────────────────────
set(_S "${CMAKE_CURRENT_SOURCE_DIR}")

# com/  (Command.cpp and SystemCommand.cpp excluded: depend on deleted SysTimer class)
set(_COM_SRCS
    ${_S}/com/Events.cpp
    ${_S}/com/Legacy.cpp
    ${_S}/com/Log.cpp
    ${_S}/com/MyString.cpp
    ${_S}/com/one4All.cpp
    ${_S}/com/PluginCollection.cpp
    ${_S}/com/PPIError.cpp
    ${_S}/com/Settings.cpp
)

# gui/
set(_GUI_SRCS
    ${_S}/gui/FrxControl.cpp
    ${_S}/gui/IViewModelMap.cpp
    ${_S}/gui/ViewFactory.cpp
    ${_S}/gui/ViewModelMap.cpp
    ${_S}/gui/ViewObject.cpp
    ${_S}/gui/components/About.cpp
    ${_S}/gui/components/FrxCircuidView.cpp
    ${_S}/gui/components/FrxColumnBrowser.cpp
    ${_S}/gui/components/FrxComponent.cpp
    ${_S}/gui/components/FrxComponentFactory.cpp
    ${_S}/gui/components/FrxConcreteParameter.cpp
    ${_S}/gui/components/FrxConcreteProcessor.cpp
    ${_S}/gui/components/FrxConnectionBrowser.cpp
    ${_S}/gui/components/FrxConnectionBrowserCtrl.cpp
    ${_S}/gui/components/FrxConnection.cpp
    ${_S}/gui/components/FrxFlag.cpp
    ${_S}/gui/components/FrxFontCache.cpp
    ${_S}/gui/components/FrxHover.cpp
    ${_S}/gui/components/FrxIO.cpp
    ${_S}/gui/components/FrxMainBrowser.cpp
    ${_S}/gui/components/FrxMainBrowserCtrl.cpp
    ${_S}/gui/components/FrxMenuLabel.cpp
    ${_S}/gui/components/FrxNode.cpp
    ${_S}/gui/components/FrxPacket.cpp
    ${_S}/gui/components/FrxParameter.cpp
    ${_S}/gui/components/FrxParameterLabel.cpp
    ${_S}/gui/components/FrxPluginBrowser.cpp
    ${_S}/gui/components/FrxPluginBrowserCtrl.cpp
    ${_S}/gui/components/FrxPluginEditor.cpp
    ${_S}/gui/components/FrxPluginEditorCtrl.cpp
    ${_S}/gui/components/FrxProcessorBrowser.cpp
    ${_S}/gui/components/FrxProcessorBrowserCtrl.cpp
    ${_S}/gui/components/FrxProcessorEditor.cpp
    ${_S}/gui/components/FrxProcessorNode.cpp
    ${_S}/gui/components/FrxScriptPluginEditor.cpp
    ${_S}/gui/components/FrxSelection.cpp
    ${_S}/gui/components/FrxSerializationRegister.cpp
    ${_S}/gui/components/FrxStatusBar.cpp
    ${_S}/gui/components/FrxTooltipManager.cpp
    ${_S}/gui/components/HorizontalFormatter.cpp
    ${_S}/gui/components/SetupCtrl.cpp
    ${_S}/gui/components/SetupWindow.cpp
    ${_S}/gui/components/ShellPluginSelection.cpp
    ${_S}/gui/components/VerticalFormatter.cpp
    ${_S}/gui/components/VstForxEditor.cpp
    ${_S}/gui/components/legacy/FrxSerializationRegister.cpp
    ${_S}/gui/components/ui/FrxBrowserListUI.cpp
    ${_S}/gui/components/ui/FrxCircuidMouseListener.cpp
    ${_S}/gui/components/ui/FrxCircuidViewUI.cpp
    ${_S}/gui/components/ui/FrxColumnBrowserUI.cpp
    ${_S}/gui/components/ui/FrxComponentUI.cpp
    ${_S}/gui/components/ui/FrxConnectionUI.cpp
    ${_S}/gui/components/ui/FrxFlagUI.cpp
    ${_S}/gui/components/ui/FrxHoverMouseListener.cpp
    ${_S}/gui/components/ui/FrxHoverUI.cpp
    ${_S}/gui/components/ui/FrxKnobUI.cpp
    ${_S}/gui/components/ui/FrxLookAndFeel.cpp
    ${_S}/gui/components/ui/FrxNodeUI.cpp
    ${_S}/gui/components/ui/FrxPacketUI.cpp
    ${_S}/gui/components/ui/FrxParameterLabelUI.cpp
    ${_S}/gui/components/ui/FrxParameterUI.cpp
    ${_S}/gui/components/ui/FrxProcessorNodeUI.cpp
    ${_S}/gui/components/ui/FrxSelectionMouseListener.cpp
    ${_S}/gui/components/ui/FrxSelectionUI.cpp
    ${_S}/gui/components/ui/FrxSvgComponentUI.cpp
    ${_S}/gui/components/ui/FrxSvgIOUI.cpp
    ${_S}/gui/components/ui/FrxSvgKnobUI.cpp
    ${_S}/gui/components/ui/FrxSvgNodeUI.cpp
    ${_S}/gui/components/ui/FrxSvgPacketUI.cpp
    ${_S}/gui/components/ui/FrxSvgPluginUI.cpp
    ${_S}/gui/components/ui/FrxSvgProcessorUI.cpp
    ${_S}/gui/components/ui/TooltipTexts.cpp
)

# processing/
set(_PROC_SRCS
    ${_S}/processing/AudioBuffer.cpp
    ${_S}/processing/ConcreteProcessAdapter.cpp
    ${_S}/processing/dspTools.cpp
    ${_S}/processing/Frames.cpp
    ${_S}/processing/FrxAsyncDSPTimer.cpp
    ${_S}/processing/Graph.cpp
    ${_S}/processing/IConnection.cpp
    ${_S}/processing/IModelController.cpp
    ${_S}/processing/INode.cpp
    ${_S}/processing/IParameter.cpp
    ${_S}/processing/IPluginAdapter.cpp
    ${_S}/processing/IProcessor.cpp
    ${_S}/processing/MidiEventProcessor.cpp
    ${_S}/processing/ModelController.cpp
    ${_S}/processing/ModelFactory.cpp
    ${_S}/processing/ModelObject.cpp
    ${_S}/processing/NodeAdapter.cpp
    ${_S}/processing/NodeConnection.cpp
    ${_S}/processing/ParameterAdapter.cpp
    ${_S}/processing/ParameterConnection.cpp
    ${_S}/processing/Plugin.cpp
    ${_S}/processing/PluginAdapter.cpp
    ${_S}/processing/PObject.cpp
    ${_S}/processing/processing.cpp
    ${_S}/processing/ProcessorAdapter.cpp
    ${_S}/processing/SerializationRegister.cpp
    ${_S}/processing/VstForxPlug.cpp
    ${_S}/processing/concreteAdapter/ADSRTrigger.cpp
    ${_S}/processing/concreteAdapter/DCTester.cpp
    ${_S}/processing/concreteAdapter/FixTimeTranslator.cpp
    ${_S}/processing/concreteAdapter/FrqDetector.cpp
    ${_S}/processing/concreteAdapter/InputStep.cpp
    ${_S}/processing/concreteAdapter/InputSwitch.cpp
    ${_S}/processing/concreteAdapter/MidiProcessor.cpp
    ${_S}/processing/concreteAdapter/OutputStep.cpp
    ${_S}/processing/concreteAdapter/OutputSwitch.cpp
    ${_S}/processing/concreteAdapter/Pan.cpp
    ${_S}/processing/concreteAdapter/PeakTracker.cpp
    ${_S}/processing/concreteAdapter/Step.cpp
    ${_S}/processing/concreteAdapter/Switch.cpp
    ${_S}/processing/concreteAdapter/SyncTranslator.cpp
    ${_S}/processing/concreteAdapter/ValueTranslator.cpp
    ${_S}/processing/concreteAdapter/Volume.cpp
    ${_S}/processing/legacy/Plugin.cpp
    ${_S}/processing/legacy/SerializationRegister.cpp
    ${_S}/processing/legacy/SerializationRegister_BIGOBJ_BUDGE1.cpp
    ${_S}/processing/legacy/SerializationRegister_BIGOBJ_BUDGE2.cpp
    ${_S}/processing/legacy/SerializationRegister_BIGOBJ_BUDGE3.cpp
    ${_S}/processing/legacy/VSTPlugin2x.cpp
    ${_S}/processing/parameter/ConnectionOperators.cpp
    ${_S}/processing/parameter/parameter.cpp
    ${_S}/processing/pluginTypes/LuaImpl.cpp
    ${_S}/processing/pluginTypes/LuaImplBase.cpp
    ${_S}/processing/pluginTypes/NullAEffect.cpp
    ${_S}/processing/pluginTypes/PluginFactory.cpp
    ${_S}/processing/pluginTypes/PluginImpl.cpp
    ${_S}/processing/pluginTypes/TestAEffect.cpp
    ${_S}/processing/pluginTypes/VST2xImpl.cpp
)

# scripts/
set(_SCRIPT_SRCS
    ${_S}/scripts/LuaFrxConnection.cpp
    ${_S}/scripts/LuaFrxConnectionBase.cpp
    ${_S}/scripts/LuaFrxIO.cpp
    ${_S}/scripts/LuaFrxIOBase.cpp
    ${_S}/scripts/LuaFrxListWindow.cpp
    ${_S}/scripts/LuaFrxListWindowBase.cpp
    ${_S}/scripts/LuaFrxObject.cpp
    ${_S}/scripts/LuaFrxObjectBase.cpp
    ${_S}/scripts/LuaFrxParameter.cpp
    ${_S}/scripts/LuaFrxParameterBase.cpp
    ${_S}/scripts/LuaFrxParameterConnection.cpp
    ${_S}/scripts/LuaFrxParameterConnectionBase.cpp
    ${_S}/scripts/LuaFrxProcessor.cpp
    ${_S}/scripts/LuaFrxProcessorBase.cpp
    ${_S}/scripts/LuaFrxView.cpp
    ${_S}/scripts/LuaFrxViewBase.cpp
    ${_S}/scripts/LuaFrxWindow.cpp
    ${_S}/scripts/LuaFrxWindowBase.cpp
    ${_S}/scripts/LuaModelObject.cpp
    ${_S}/scripts/LuaModelObjectBase.cpp
    ${_S}/scripts/LuaParameter.cpp
    ${_S}/scripts/LuaParameterBase.cpp
    ${_S}/scripts/LuaTimer.cpp
    ${_S}/scripts/LuaTimerBase.cpp
    ${_S}/scripts/LuaUserData.cpp
    ${_S}/scripts/PluginScriptCtrl.cpp
)

# OS-specific sources
if(WIN32)
    set(_OS_SRCS
        ${_S}/OS_Specific/windows/com/win_one4All.cpp
        ${_S}/OS_Specific/windows/processing/OS_VSTPlugin2x.cpp
    )
elseif(APPLE)
    set(_OS_SRCS
        ${_S}/OS_Specific/mac/com/mac_one4All.cpp
        ${_S}/OS_Specific/mac/processing/OS_VSTPlugin2x.cpp
    )
elseif(UNIX)
    set(_OS_SRCS
        ${_S}/OS_Specific/linux/com/linux_one4All.cpp
        ${_S}/OS_Specific/linux/processing/OS_VSTPlugin2x.cpp
    )
endif()

# VST3 sources (optional)
if(FRX_BUILD_VST3)
    list(APPEND _PROC_SRCS
        ${_S}/processing/pluginTypes/VST3xImpl.cpp
        ${_S}/processing/pluginTypes/VST3ParameterChanges.cpp
    )
else()
    list(APPEND _PROC_SRCS
        ${_S}/processing/pluginTypes/VST3xImplStub.cpp
    )
endif()

# VST2 SDK sources (always needed: AudioEffectX base class)
set(_VSTSDK2_SRCS
    ${FRX_VSTSDK2_DIR}/public.sdk/source/vst2.x/audioeffect.cpp
    ${FRX_VSTSDK2_DIR}/public.sdk/source/vst2.x/audioeffectx.cpp
)

# sambag DSP helpers that depend on VST2 SDK (excluded from sambag build when SAMBAG_VST_SUPPORT=OFF)
set(_SAMBAG_DSP_VST_SRCS
    "${SAMBAG_DIR}/src/sambag/dsp/VstMidiEventAdapter.cpp"
    "${SAMBAG_DIR}/src/sambag/dsp/TimeInfoVst2xHelper.cpp"
)

# ── frx_core library ──────────────────────────────────────────────────────────
add_library(frx_core STATIC
    ${_COM_SRCS}
    ${_GUI_SRCS}
    ${_PROC_SRCS}
    ${_SCRIPT_SRCS}
    ${_OS_SRCS}
    ${_VSTSDK2_SRCS}
    ${_SAMBAG_DSP_VST_SRCS}
)

target_include_directories(frx_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    # FrxConfig.h is generated in the vstforx/vstforx binary dir
    # <com/FrxConfig.h> needs the parent of com/:
    "${CMAKE_CURRENT_BINARY_DIR}"
    # "FrxConfig.h" (no prefix) in Settings.cpp needs com/ directly:
    "${CMAKE_CURRENT_BINARY_DIR}/com"
    # VST2 SDK include paths
    "${FRX_VSTSDK2_DIR}"
    "${FRX_VSTSDK2_DIR}/public.sdk/source/vst2.x"
    "${FRX_VSTSDK2_DIR}/pluginterfaces/vst2.x"
    # Loki (header-only, found by sambag)
    "${LOKI_INCLUDE_DIR}"
)

target_link_libraries(frx_core PUBLIC
    ${SAMBAG_LIBRARIES}
    Boost::filesystem
    Boost::system
    Boost::thread
    Boost::regex
    Boost::serialization
    Boost::date_time
    Boost::timer
    Boost::chrono
    Boost::program_options
    ${FRX_CAIRO_LIBRARIES}
    ${FRX_LUA_LIBRARIES}
)

if(UNIX AND NOT APPLE)
    target_link_libraries(frx_core PUBLIC ${X11_LIBRARIES} rt)
elseif(APPLE)
    target_link_libraries(frx_core PUBLIC ${COCOA_LIBRARY} ${CARBON_LIBRARY})
elseif(WIN32)
    target_link_libraries(frx_core PUBLIC ws2_32 mswsock)
endif()

target_include_directories(frx_core PUBLIC ${SAMBAG_INCLUDE_DIRS})

# Cairo include (needed for frx_core headers that use cairo directly)
if(WIN32)
    target_include_directories(frx_core PUBLIC
        "${CAIRO_INCLUDE_DIR}"
        "${CAIRO_INCLUDE_DIR}/cairo"
    )
else()
    # pkg-config Cairo adds its own include dirs via PkgConfig::Cairo
endif()

add_subdirectory(PluginApps)
