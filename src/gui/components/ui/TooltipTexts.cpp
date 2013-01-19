/*
 * TooltipTexts.cpp
 *
 *  Created on: Mon Nov 26 14:16:18 2012
 *      Author: Johannes Unger
 */

#include "TooltipTexts.hpp"

#include <gui/components/FrxConcreteProcessor.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/FrxConcreteConnections.hpp>
#include <gui/components/FrxConcreteIO.hpp>
#include <gui/components/FrxConcreteParameter.hpp>
#include <boost/assign.hpp>

namespace {
const std::string NULL_TOOLTIPTXT = "";
template <class T>
inline Loki::TypeInfo _typeInfo() {
	return Loki::TypeInfo(typeid(T));
}

} // namespace(s)
namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class TooltipTexts
//=============================================================================
//-----------------------------------------------------------------------------
TooltipTexts::TypeMap TooltipTexts::typeMap;
//-----------------------------------------------------------------------------
void TooltipTexts::initMap() {
	using namespace boost::assign;
	typeMap = map_list_of
	(_typeInfo<FrxInputNode>(), "")
	(_typeInfo<FrxOutputNode>(), "")
	(_typeInfo<FrxEntryNode>(), "Main signal entry.")
	(_typeInfo<FrxExitNode>(), "Main signal exit.")
	(_typeInfo<FrxCircuidView>(), "Use the contextmenu.")
	(_typeInfo<FrxPluginNode>(), "Plugin")
	(_typeInfo<FrxVolumeNode>(), "Volume")
	(_typeInfo<FrxPanNode>(), "Pan")
	(_typeInfo<FrxInStepNode>(), "InStep")
	(_typeInfo<FrxOutStepNode>(), "OutStep")
	(_typeInfo<FrxInSwitchNode>(), "InSwitch")
	(_typeInfo<FrxOutSwitchNode>(), "OutSwitch")
	(_typeInfo<FrxADSRNode>(), "ADSRNode")
	(_typeInfo<FrxPeakTrackerNode>(), "PeakTracker");
}
//-----------------------------------------------------------------------------
const std::string & TooltipTexts::_getTooltipText(const Loki::TypeInfo &typeinfo) 
{
	if (typeMap.empty()) {
		initMap();
	}
	TypeMap::const_iterator it = typeMap.find(typeinfo);
	if (it==typeMap.end())
		return NULL_TOOLTIPTXT;
	return it->second;
}
}}}} // namespace(s)
