/*
 * FrxPluginNode.hpp
 *
 *  Created on: Mon Aug 20 12:08:05 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPLUGINNODE_H
#define SAMBAG_FRXPLUGINNODE_H

#include <boost/shared_ptr.hpp>
#include "FrxProcessorNode.hpp"

namespace frx { namespace gui { namespace components {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxPluginNode.
  */
class FrxPluginNode : public FrxProcessorNode {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxProcessorNode Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxPluginNode> Ptr;
	//-------------------------------------------------------------------------
	virtual sdcu::AComponentUIPtr getComponentUI(sdcu::ALookAndFeelPtr laf) const;
protected:
	//-------------------------------------------------------------------------
	FrxPluginNode();
private:
public:
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(FrxPluginNode)
}; // FrxPluginNode
}}} // namespace(s)

#endif /* SAMBAG_FRXPLUGINNODE_H */
