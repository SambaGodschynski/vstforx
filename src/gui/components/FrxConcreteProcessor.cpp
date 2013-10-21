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


namespace __private {

///////////////////////////////////////////////////////////////////////////////
// Processor Names
const char *ProcessorNames[] = {"FrxPlugin",
                                "FrxVolume",
                                "FrxPan",
                                "FrxInStep",
                                "FrxOutStep",
                                "FrxInSwitch",
                                "FrxOutSwitch",
                                "FrxADSR",
                                "FrxPeakTracker",
                                "FrxMidiReceiver",
                                "FrxRemoteChReceiver",
                                "FrxDCTester"};

const size_t NumProcessorNames = sizeof(ProcessorNames) / sizeof(ProcessorNames[0]);
BOOST_STATIC_ASSERT( NumProcessorNames == Loki::TL::Length<FrxProcessorList>::value );

const char * _getProcessorNameImpl(size_t index) {
    return ProcessorNames[index];
}
///////////////////////////////////////////////////////////////////////////////
// Beauty Names
const char *ProcessorBNames[] = { "Plugin",
                                  "Volume",
                                  "Pan",
                                  "Input Step",
                                  "Output Step",
                                  "Input Switch",
                                   "Output Switch",
                                  "ADSR Trigger",
                                  "Peak Tracker",
                                  "Midi Receiver",
                                  "Remote Channel Receiver",
                                  "DC Tester"};

const size_t NumProcessorBNames = sizeof(ProcessorBNames) / sizeof(ProcessorBNames[0]);
BOOST_STATIC_ASSERT( NumProcessorBNames == Loki::TL::Length<FrxProcessorList>::value );

const char * _getProcessorBeautyNameImpl(size_t index) {
    return ProcessorBNames[index];
}
///////////////////////////////////////////////////////////////////////////////
// Tooltips
const char *ProcessorTips[] = { "click the (e) to open/close plug's editor.",
                                "sets volume", "sets panning",
                                "steps through several inputs",
                                "steps through several outputs",
                                "switchs several inputs",
                                "switchs several outputs",
                                "creates an ASDR sequence triggered by an input audio event",
                                "transforms audio peaks into parameter values",
                                "transform midi events into parameter values",
                                "receives remote channel data",
                                "adds delay between input and output"};
    
const size_t NumProcessorTips = sizeof(ProcessorTips) / sizeof(ProcessorTips[0]);
BOOST_STATIC_ASSERT( NumProcessorTips == Loki::TL::Length<FrxProcessorList>::value );
    
const char * _getProcessorTooltipImpl(size_t index) {
    return ProcessorTips[index];
}

} // namespace(s)
//-----------------------------------------------------------------------------
namespace {
	typedef boost::function <std::string()> GetStrF;
	typedef boost::tuple<GetStrF, GetStrF> NameFs;
	typedef boost::unordered_map<std::string, NameFs> ProcessorNameMap;
	ProcessorNameMap processorNameMap;
	template <class Pr>
	void addProcessor() {
		GetStrF beauty = &getProcessorBeautyName<Pr>;
		GetStrF toolt = &getProcessorTooltip<Pr>;
		processorNameMap.insert( ProcessorNameMap::value_type(
				getProcessorName<Pr>(),
				boost::make_tuple(beauty, toolt)
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
