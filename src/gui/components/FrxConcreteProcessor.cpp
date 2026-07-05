/*
 * FrxConcreteProcessor.cpp
 *
 *  Created on: Mon Aug 20 12:12:39 2012
 *      Author: Johannes Unger
 */

#include "FrxConcreteProcessor.hpp"
#include <functional>
#include <tuple>
#include <unordered_map>
#include <sambag/disco/IResourceManager.hpp>

namespace frx { namespace gui { namespace components {
namespace processorTypes {
void Plugin::init( FrxProcessorNode::Ptr obj ) {
}
///////////////////////////////////////////////////////////////////////////////
// Processor Details
const char * Plugin::Details::name           = "Plugin";
const char * Volume::Details::name           = "Volume";
const char * Pan::Details::name              = "Pan";
const char * InStep::Details::name           = "InputStep";
const char * OutStep::Details::name          = "OutputStep";
const char * InSwitch::Details::name         = "InputSwitch";
const char * OutSwitch::Details::name        = "OutputSwitch";
const char * ADSR::Details::name             = "ADSRTrigger";
const char * PeakTracker::Details::name      = "PeakTracker";
const char * MIDIReceiver::Details::name     = "MidiProcessor";
const char * RemoteChReceiver::Details::name = "RemoteChReceiver";
const char * DCTester::Details::name         = "DCTester";
const char * FrqDetector::Details::name         = "FrqDetector";


const char * Plugin::Details::beautyName           = "Plugin";
const char * Volume::Details::beautyName           = "Volume";
const char * Pan::Details::beautyName              = "Pan";
const char * InStep::Details::beautyName           = "Input Step";
const char * OutStep::Details::beautyName          = "Output Step";
const char * InSwitch::Details::beautyName         = "Input Switch";
const char * OutSwitch::Details::beautyName        = "Output Switch";
const char * ADSR::Details::beautyName             = "ADSR Trigger";
const char * PeakTracker::Details::beautyName      = "Peak Tracker";
const char * MIDIReceiver::Details::beautyName     = "Midi Receiver";
const char * RemoteChReceiver::Details::beautyName = "Remote Channel Receiver";
const char * DCTester::Details::beautyName         = "DC Tester";
const char * FrqDetector::Details::beautyName      = "Frequency Detector";

const char * Plugin::Details::toolTip           = "click the (e) to open/close plug's editor.";
const char * Volume::Details::toolTip           = "changes volume";
const char * Pan::Details::toolTip              = "changes panning";
const char * InStep::Details::toolTip           = "steps through several inputs";
const char * OutStep::Details::toolTip          = "steps through several outputs";
const char * InSwitch::Details::toolTip         = "switchs several inputs";
const char * OutSwitch::Details::toolTip        = "switchs several outputs";
const char * ADSR::Details::toolTip             = "creates an ASDR sequence triggered by an input audio event";
const char * PeakTracker::Details::toolTip      = "transforms audio peaks into parameter values";
const char * MIDIReceiver::Details::toolTip     = "transform midi events into parameter values";
const char * RemoteChReceiver::Details::toolTip = "receives remote channel data";
const char * DCTester::Details::toolTip         = "adds delay between input and output";
const char * FrqDetector::Details::toolTip      = "Determines the frequency of a signal";

const char * Plugin::Details::ns           = "?";
const char * Volume::Details::ns           = "internal";
const char * Pan::Details::ns              = "internal";
const char * InStep::Details::ns           = "internal";
const char * OutStep::Details::ns          = "internal";
const char * InSwitch::Details::ns         = "internal";
const char * OutSwitch::Details::ns        = "internal";
const char * ADSR::Details::ns             = "internal";
const char * PeakTracker::Details::ns      = "internal";
const char * MIDIReceiver::Details::ns     = "internal";
const char * RemoteChReceiver::Details::ns = "interprocess";
const char * DCTester::Details::ns         = "internal-private";
const char * FrqDetector::Details::ns      = "internal";

} // namespace processorTypes
//-----------------------------------------------------------------------------
namespace {
	typedef std::function<const char *()> GetStrF;
	typedef std::tuple<GetStrF, GetStrF> NameFs;
	typedef std::unordered_map<std::string, NameFs> ProcessorNameMap;
	ProcessorNameMap processorNameMap;
	template <class Pr>
	void addProcessor() {
		GetStrF beauty = &getProcessorBeautyName<Pr>;
		GetStrF toolt = &getProcessorTooltip<Pr>;
		processorNameMap.insert( ProcessorNameMap::value_type(
				getProcessorName<Pr>(),
				std::make_tuple(beauty, toolt)
			)
		);
	}
    template <class Types>
    void addProcessors() {
        addProcessor<typename Types::Head>();
        addProcessors<typename Types::Tail>();
    }
    template <>
    void addProcessors<Loki::NullType>() {}
	void initNameMap() {
 		addProcessors<FrxProcessorList>();
    }
	ProcessorNameMap::const_iterator getPMapIterator(const std::string &processorName)
	{
		if (processorNameMap.size()==0) {
			initNameMap();
		}
		return processorNameMap.find(processorName);
	}
}
//-----------------------------------------------------------------------------
std::string getProcessorBeautyName(const std::string &processorName) {
	ProcessorNameMap::const_iterator it = getPMapIterator(processorName);
	if (it==processorNameMap.end()) {
        return "unkown processor";
	}
	return std::get<0>( it->second )();
}
//-----------------------------------------------------------------------------
std::string getProcessorTooltip(const std::string &processorName) {
	ProcessorNameMap::const_iterator it = getPMapIterator(processorName);
	if (it==processorNameMap.end()) {
		return "?";
	}
	return std::get<1>( it->second )();
}
}}} // namespace(s)

#include <com/Serialization.h>
namespace frx { namespace gui { namespace components {
namespace {
    bool FrxProcessors_Registered = registerInFactory<FrxProcessorList>();
}
}}} // namespace frx::gui::components

namespace frx { namespace gui { namespace components {
namespace {
    template <>
    bool _doRegister<FrxPluginNode>() {
        typedef FrxPluginNode T;
        return ViewFactory::instance().register_<T>(
            std::string("vst2x.Plugin"),   &T::create
        ) && ViewFactory::instance().register_<T>(
            std::string("vst3x.Plugin"),   &T::create
        ) && ViewFactory::instance().register_<T>(
            std::string("au.Plugin"),      &T::create
        ) && ViewFactory::instance().register_<T>(
            std::string("dx.Plugin"),      &T::create
        ) && ViewFactory::instance().register_<T>(
            std::string("lua.Plugin"),     &T::create
        ) && ViewFactory::instance().register_<T>(
            std::string("unknown-plugin.Plugin"),  &T::create
        ) && ViewFactory::instance().register_<T>(
            std::string("bridged-plugin.Plugin"),  &T::create
        );
    }
}
}}} // namespace frx::gui::components
