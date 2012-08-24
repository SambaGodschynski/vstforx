/*
 * FrxProcessorNode.hpp
 *
 *  Created on: Mon Aug 20 10:45:09 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPROCESSORNODE_H
#define SAMBAG_FRXPROCESSORNODE_H

#include <boost/shared_ptr.hpp>
#include "FrxNode.hpp"

namespace frx { namespace gui { namespace components {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxProcessorNode.
  */
class FrxProcessorNode : public FrxNode {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxNode Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxProcessorNode> Ptr;
	//-------------------------------------------------------------------------
	virtual sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const;
protected:
	//-------------------------------------------------------------------------
	FrxProcessorNode();
private:
public:
}; // FrxProcessorNode
}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSORNODE_H */
