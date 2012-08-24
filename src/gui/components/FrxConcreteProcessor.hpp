/*
 * FrxConcreteProcessor.hpp
 *
 *  Created on: Mon Aug 20 12:08:05 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPLUGINNODE_H
#define SAMBAG_FRXPLUGINNODE_H

#include <boost/shared_ptr.hpp>
#include "FrxProcessorNode.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>

namespace frx { namespace gui { namespace components {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxConcreteProcessor.
  */
template <class _ProcessorType>
class FrxConcreteProcessor : 
	public FrxProcessorNode, 
	public _ProcessorType 
{
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxProcessorNode Super;
	//-------------------------------------------------------------------------
	typedef _ProcessorType ProcessorType;
	//-------------------------------------------------------------------------
	typedef FrxConcreteProcessor<ProcessorType> ConcreteProcessor;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ConcreteProcessor> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxConcreteProcessor() {
		setName("FrxConcreteProcessor");
	}
	//-------------------------------------------------------------------------
	virtual void postConstructor() {
		ProcessorType::init( getPtr() );
	}
private:
public:
	//-------------------------------------------------------------------------
	sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const
	{
		return laf->getUI<ConcreteProcessor>();
	}
	//-------------------------------------------------------------------------
	Ptr getPtr() const {
		return boost::shared_dynamic_cast<ConcreteProcessor>(Super::getPtr());
	}
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res( new ConcreteProcessor() );
		res->self = res;
		res->postConstructor();
		return res;
	}
}; // FrxConcreteProcessor
//=============================================================================
// Types
//=============================================================================
namespace processorTypes {
	struct ProcessorTypeBase{ void init( FrxProcessorNode::Ptr ){} };
	struct Plugin : ProcessorTypeBase{};
	
}
typedef FrxConcreteProcessor<processorTypes::Plugin> FrxPluginNode;
}}} // namespace(s)

#endif /* SAMBAG_FRXPLUGINNODE_H */
