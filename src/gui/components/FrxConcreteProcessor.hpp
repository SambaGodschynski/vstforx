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
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
//-----------------------------------------------------------------------------
template <class _ProcessorType>
std::string getProcessorName();
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
		setName(getProcessorName<ProcessorType>());
	}
	//-------------------------------------------------------------------------
	virtual void postConstructor() {
		Super::postConstructor();
		ProcessorType::init( getPtr() );
	}
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) { 
		ar & boost::serialization::base_object<Super>(*this); 
	} 
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
	struct Volume : ProcessorTypeBase{};
	struct Pan : ProcessorTypeBase{};
	struct InStep : ProcessorTypeBase{};
	struct OutStep : ProcessorTypeBase{};
	struct InSwitch : ProcessorTypeBase{};
	struct OutSwitch : ProcessorTypeBase{};
	struct ADSR : ProcessorTypeBase{};
	struct PeakTracker : ProcessorTypeBase{};
	
}
typedef FrxConcreteProcessor<processorTypes::Plugin> FrxPluginNode;
typedef FrxConcreteProcessor<processorTypes::Volume> FrxVolumeNode;
typedef FrxConcreteProcessor<processorTypes::Pan> FrxPanNode;
typedef FrxConcreteProcessor<processorTypes::InStep> FrxInStepNode;
typedef FrxConcreteProcessor<processorTypes::OutStep> FrxOutStepNode;
typedef FrxConcreteProcessor<processorTypes::InSwitch> FrxInSwitchNode;
typedef FrxConcreteProcessor<processorTypes::OutSwitch> FrxOutSwitchNode;
typedef FrxConcreteProcessor<processorTypes::ADSR> FrxADSRNode;
typedef FrxConcreteProcessor<processorTypes::PeakTracker> FrxPeakTrackerNode;
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class _ProcessorType>
std::string getProcessorName() {return "unkonwn processortype";}
template <>
inline std::string getProcessorName<processorTypes::Plugin>() {return "FrxPlugin";}
template <>
inline std::string getProcessorName<processorTypes::Volume>() {return "FrxVolume";}
template <>
inline std::string getProcessorName<processorTypes::Pan>() {return "FrxPan";}
template <>
inline std::string getProcessorName<processorTypes::InStep>() {return "FrxInStep";}
template <>
inline std::string getProcessorName<processorTypes::OutStep>() {return "FrxOutStep";}
template <>
inline std::string getProcessorName<processorTypes::InSwitch>() {return "FrxInSwitch";}
template <>
inline std::string getProcessorName<processorTypes::OutSwitch>() {return "FrxOutSwitch";}
template <>
inline std::string getProcessorName<processorTypes::ADSR>() {return "FrxADSR";}
template <>
inline std::string getProcessorName<processorTypes::PeakTracker>() {return "FrxPeakTracker";}
}}} // namespace(s)

#endif /* SAMBAG_FRXPLUGINNODE_H */
