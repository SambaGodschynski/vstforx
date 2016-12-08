/*
 * PluginImpl.cpp
 *
 *  Created on: Tue Jan  7 13:30:58 2014
 *      Author: Johannes Unger
 */

#include "PluginImpl.hpp"
#include <sstream>
#include "com/Serialization.h"

namespace frx { namespace processing { 
//=============================================================================
//  Class PluginImpl
//=============================================================================
//-----------------------------------------------------------------------------
APluginImpl::APluginImpl(IHostInfo::Ptr hI, const std::string &location,
        Parameters *parameters) : hostInfo(hI),
                                  parameters(parameters),
                                  location(location)
{
}
//-----------------------------------------------------------------------------
oldPr::IMidiEventProcessor::Connection
APluginImpl::addMidiEventListener(const oldPr::IMidiEventProcessor::EventFunction &f)
{
    return oldPr::IMidiEventProcessor::EventSender::addEventListener(f);
}
//-----------------------------------------------------------------------------
oldPr::IMidiEventProcessor::Connection
APluginImpl::addTrackedMidiEventListener(const oldPr::IMidiEventProcessor::EventFunction &f,
    oldPr::IMidiEventProcessor::AnyWPtr trackingObject)
{
    return oldPr::IMidiEventProcessor::EventSender::addTrackedEventListener(f, trackingObject);
}
//-----------------------------------------------------------------------------
void APluginImpl::setPresetData(const std::string &strData) {
	std::stringstream ss;
	size_t size;
	ss << strData;
	::com::iArchive ar(ss);
	ar >> size;
	if (size == 0) {
		return;
	}
	unsigned char *data[1] = { new unsigned char[size] };
	ar.load_binary(*data, size);
	setStateData(size, data[0]);
}
//-----------------------------------------------------------------------------
std::string APluginImpl::getPresetData() {
	std::stringstream ss;
	::com::oArchive ar(ss);
	std::pair<size_t, void*> state = getStateData();
	ar << state.first;
	ar.save_binary(state.second, state.first);
	return ss.str();
}

}} // namespace(s)
