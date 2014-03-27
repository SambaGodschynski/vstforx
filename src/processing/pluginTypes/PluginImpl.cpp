/*
 * PluginImpl.cpp
 *
 *  Created on: Tue Jan  7 13:30:58 2014
 *      Author: Johannes Unger
 */

#include "PluginImpl.hpp"

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
}} // namespace(s)
