/*
 * FrxConcreteProcessor.cpp
 *
 *  Created on: Mon Aug 20 12:12:39 2012
 *      Author: Johannes Unger
 */

#include "FrxConcreteProcessor.hpp"
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/unordered_map.hpp>
#include <sambag/disco/IResourceManager.hpp>

namespace frx { namespace gui { namespace components {
namespace processorTypes {
void Plugin::init( FrxProcessorNode::Ptr obj ) {
	sdc::Button::Ptr btn(sdc::Button::create());
	btn->setIcon(
		sd::getResourceManager().getImage("FrxPlugin.e.image")
	);
	btn->setSize(sd::Dimension(30., 30.));
	btn->setFont( btn->getFont().setSize(25.) );
	btn->setTooltipText("click the (e) to open/close plug's editor.");
	//btn->putClientProperty("Button.cornerRadius", sd::Coordinate(10.));
	btn->putClientProperty("Button.drawRect", false);
	btn->setInheritsPopupMenu(true);
	obj->add(btn);
}
} // processorTypes

///////////////////////////////////////////////////////////////////////////////
// Processor Names
template <>
std::string getProcessorName<processorTypes::Plugin>() {return "FrxPlugin";}
template <>
std::string getProcessorName<processorTypes::Volume>() {return "FrxVolume";}
template <>
std::string getProcessorName<processorTypes::Pan>() {return "FrxPan";}
template <>
std::string getProcessorName<processorTypes::InStep>() {return "FrxInStep";}
template <>
std::string getProcessorName<processorTypes::OutStep>() {return "FrxOutStep";}
template <>
std::string getProcessorName<processorTypes::InSwitch>() {return "FrxInSwitch";}
template <>
std::string getProcessorName<processorTypes::OutSwitch>() {return "FrxOutSwitch";}
template <>
std::string getProcessorName<processorTypes::ADSR>() {return "FrxADSR";}
template <>
std::string getProcessorName<processorTypes::PeakTracker>() {return "FrxPeakTracker";}
template <>
std::string getProcessorName<processorTypes::MIDIReceiver>() {return "FrxMidiReceiver";}
template <>
std::string getProcessorName<processorTypes::RemoteChReceiver>() {return "FrxRemoteChReceiver";}
///////////////////////////////////////////////////////////////////////////////
// Beauty Names
template <>
std::string getProcessorBeautyName<processorTypes::Plugin>() {return "Plugin";}
template <>
std::string getProcessorBeautyName<processorTypes::Volume>() {return "Volume";}
template <>
std::string getProcessorBeautyName<processorTypes::Pan>() {return "Pan";}
template <>
std::string getProcessorBeautyName<processorTypes::InStep>() {return "Input Step";}
template <>
std::string getProcessorBeautyName<processorTypes::OutStep>() {return "Output Step";}
template <>
std::string getProcessorBeautyName<processorTypes::InSwitch>() {return "Input Switch";}
template <>
std::string getProcessorBeautyName<processorTypes::OutSwitch>() {return "Output Switch";}
template <>
std::string getProcessorBeautyName<processorTypes::ADSR>() {return "ADSR Trigger";}
template <>
std::string getProcessorBeautyName<processorTypes::PeakTracker>() {return "Peak Tracker";}
template <>
std::string getProcessorBeautyName<processorTypes::MIDIReceiver>() {return "Midi Receiver";}
template <>
std::string getProcessorBeautyName<processorTypes::RemoteChReceiver>() {return "Remote Channel Receiver";}
///////////////////////////////////////////////////////////////////////////////
// Tooltips
template <>
std::string getProcessorTooltip<processorTypes::Plugin>() 
{return "click the (e) to open/close plug's editor.";}
template <>
std::string getProcessorTooltip<processorTypes::Volume>() 
{return "sets volume";}
template <>
std::string getProcessorTooltip<processorTypes::Pan>() 
{return "sets panning";}
template <>
std::string getProcessorTooltip<processorTypes::InStep>()
{return "steps through several inputs";}
template <>
std::string getProcessorTooltip<processorTypes::OutStep>() 
{return "steps through several outputs";}
template <>
std::string getProcessorTooltip<processorTypes::InSwitch>() 
{return "switchs several inputs";}
template <>
std::string getProcessorTooltip<processorTypes::OutSwitch>() 
{return "switchs several outputs";}
template <>
std::string getProcessorTooltip<processorTypes::ADSR>() 
{return "creates an ASDR sequence triggered by an input audio event";}
template <>
std::string getProcessorTooltip<processorTypes::PeakTracker>() 
{return "transforms audio peaks into parameter values";}
template <>
std::string getProcessorTooltip<processorTypes::MIDIReceiver>() {
return "transform midi events into parameter values";
}
template <>
std::string getProcessorTooltip<processorTypes::RemoteChReceiver>() {
return "receives remote channel data";
}
//-----------------------------------------------------------------------------
namespace {
	typedef boost::function <std::string()> GetStrF;
	typedef boost::tuple<GetStrF, GetStrF> NameFs;
	typedef boost::unordered_map<std::string, NameFs> ProcessorNameMap;
	ProcessorNameMap processorNameMap;
	template <class _Pr>
	void addProcessor() {
		typedef typename _Pr::ProcessorType Pr;
		GetStrF beauty = &getProcessorBeautyName<Pr>;
		GetStrF toolt = &getProcessorTooltip<Pr>;
		processorNameMap.insert( ProcessorNameMap::value_type(
				getProcessorName<Pr>(),
				boost::make_tuple(beauty, toolt)
			)
		);
	}
	void initNameMap() {
		addProcessor<FrxPluginNode>();
		addProcessor<FrxVolumeNode>();
		addProcessor<FrxPanNode>();
		addProcessor<FrxInStepNode>();
		addProcessor<FrxOutStepNode>();
		addProcessor<FrxInSwitchNode>();
		addProcessor<FrxOutSwitchNode>();
		addProcessor<FrxADSRNode>();
		addProcessor<FrxPeakTrackerNode>();
		addProcessor<FrxMIDIReceiver>();
        addProcessor<FrxRemoteChReceiver>();
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
	return boost::get<0>( it->second )();
}
//-----------------------------------------------------------------------------
std::string getProcessorTooltip(const std::string &processorName) {
	ProcessorNameMap::const_iterator it = getPMapIterator(processorName);
	if (it==processorNameMap.end()) {
		return "?";
	}
	return boost::get<1>( it->second )();
}
}}} // namespace(s)
