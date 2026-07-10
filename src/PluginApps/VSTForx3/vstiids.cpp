// Out-of-line definitions for VST3 interface IIDs.
// coreiids.cpp covers FUnknown/IPluginBase/IPluginFactory.
// This file covers the Vst-namespace interfaces we actually implement.

#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/gui/iplugview.h"

namespace Steinberg {
namespace Vst {

DEF_CLASS_IID(IComponent)
DEF_CLASS_IID(IAudioProcessor)
DEF_CLASS_IID(IAudioPresentationLatency)
DEF_CLASS_IID(IProcessContextRequirements)
DEF_CLASS_IID(IComponentHandler)
DEF_CLASS_IID(IComponentHandler2)
DEF_CLASS_IID(IComponentHandlerBusActivation)
DEF_CLASS_IID(IEditController)
DEF_CLASS_IID(IEditController2)
DEF_CLASS_IID(IMidiMapping)
DEF_CLASS_IID(IEditControllerHostEditing)
DEF_CLASS_IID(IComponentHandlerSystemTime)
DEF_CLASS_IID(IProgress)

} // namespace Vst
} // namespace Steinberg

namespace Steinberg {
DEF_CLASS_IID(IPlugView)
DEF_CLASS_IID(IPlugFrame)
} // namespace Steinberg
