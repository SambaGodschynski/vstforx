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
#include <sambag/disco/components/Button.hpp>

namespace frx { namespace gui { namespace components {
//-----------------------------------------------------------------------------
template <class _ProcessorType>
std::string getProcessorName();
//-----------------------------------------------------------------------------
template <class _ProcessorType>
std::string getProcessorBeautyName();
//-----------------------------------------------------------------------------
template <class _ProcessorType>
std::string getProcessorTooltip();
//-----------------------------------------------------------------------------
std::string getProcessorBeautyName(const std::string &processorName);
//-----------------------------------------------------------------------------
std::string getProcessorTooltip(const std::string &processorName);
//=============================================================================
/** 
  * @class FrxConcreteProcessor.
  * 
  * Steps to implement a new processor type:
  * 
  *  add FrxConcreteProcessor, 
  *  add createXY() IModelController method, 
  *  add FrxComponentFactory creatorMap entry and impl. related ModelExecutor function
  *  register new view type in FrxLookAndFeel
  *
  * register new view type in FrxSerializationRegister
  * register new model type in SerializationRegister
  * 
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
		instances++;
	}
	//-------------------------------------------------------------------------
	virtual void postConstructor() {
		Super::postConstructor();
		ProcessorType::init( getPtr() );
		if (getName() == "") {
			setName(getProcessorBeautyName<ProcessorType>());
			setName(getName()+"_"+sambag::com::toString(instances));
		}
		setUpperFlagText(getName());
		setTooltipText( getProcessorTooltip<ProcessorType>() );
	}
private:
	//-------------------------------------------------------------------------
	static int instances;
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
		return boost::dynamic_pointer_cast<ConcreteProcessor>(Super::getPtr());
	}
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res( new ConcreteProcessor() );
		res->self = res;
		res->postConstructor();
		return res;
	}
}; // FrxConcreteProcessor
//-----------------------------------------------------------------------------
template <class _ProcessorType>
int FrxConcreteProcessor<_ProcessorType>::instances = 0;
//=============================================================================
// Types
//=============================================================================
namespace processorTypes {
	struct ProcessorTypeBase{ void init( FrxProcessorNode::Ptr ){} };
	struct Plugin : ProcessorTypeBase{ 
		bool _isSynth;
		void init( FrxProcessorNode::Ptr obj );
		Plugin() : _isSynth(false) {}
		void isSynth(bool val) { _isSynth = val; }
		bool isSynth() const { return _isSynth; }
	};
	struct Volume : ProcessorTypeBase{};
	struct Pan : ProcessorTypeBase{};
	struct InStep : ProcessorTypeBase{};
	struct OutStep : ProcessorTypeBase{};
	struct InSwitch : ProcessorTypeBase{};
	struct OutSwitch : ProcessorTypeBase{};
	struct ADSR : ProcessorTypeBase{};
	struct PeakTracker : ProcessorTypeBase{};
	struct MIDIReceiver : ProcessorTypeBase{};
    struct RemoteChReceiver : ProcessorTypeBase{};
}

///////////////////////////////////////////////////////////////////////////////
typedef FrxConcreteProcessor<processorTypes::Plugin> FrxPluginNode;
typedef FrxConcreteProcessor<processorTypes::Volume> FrxVolumeNode;
typedef FrxConcreteProcessor<processorTypes::Pan> FrxPanNode;
typedef FrxConcreteProcessor<processorTypes::InStep> FrxInStepNode;
typedef FrxConcreteProcessor<processorTypes::OutStep> FrxOutStepNode;
typedef FrxConcreteProcessor<processorTypes::InSwitch> FrxInSwitchNode;
typedef FrxConcreteProcessor<processorTypes::OutSwitch> FrxOutSwitchNode;
typedef FrxConcreteProcessor<processorTypes::ADSR> FrxADSRNode;
typedef FrxConcreteProcessor<processorTypes::PeakTracker> FrxPeakTrackerNode;
typedef FrxConcreteProcessor<processorTypes::MIDIReceiver> FrxMIDIReceiver;
typedef FrxConcreteProcessor<processorTypes::RemoteChReceiver> FrxRemoteChReceiver;
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class _ProcessorType>
std::string getProcessorName() {return "unkonwn processortype";}
template <>
std::string getProcessorName<processorTypes::Plugin>();
template <>
std::string getProcessorName<processorTypes::Volume>();
template <>
std::string getProcessorName<processorTypes::Pan>();
template <>
std::string getProcessorName<processorTypes::InStep>();
template <>
std::string getProcessorName<processorTypes::OutStep>();
template <>
std::string getProcessorName<processorTypes::InSwitch>();
template <>
std::string getProcessorName<processorTypes::OutSwitch>();
template <>
std::string getProcessorName<processorTypes::ADSR>();
template <>
std::string getProcessorName<processorTypes::PeakTracker>();
template <>
std::string getProcessorName<processorTypes::MIDIReceiver>();
template <>
std::string getProcessorName<processorTypes::RemoteChReceiver>();
//-----------------------------------------------------------------------------
template <class _ProcessorType>
std::string getProcessorBeautyName() {return "unkonwn processortype";}
template <>
std::string getProcessorBeautyName<processorTypes::Plugin>();
template <>
std::string getProcessorBeautyName<processorTypes::Volume>();
template <>
std::string getProcessorBeautyName<processorTypes::Pan>();
template <>
std::string getProcessorBeautyName<processorTypes::InStep>();
template <>
std::string getProcessorBeautyName<processorTypes::OutStep>();
template <>
std::string getProcessorBeautyName<processorTypes::InSwitch>();
template <>
std::string getProcessorBeautyName<processorTypes::OutSwitch>();
template <>
std::string getProcessorBeautyName<processorTypes::ADSR>();
template <>
std::string getProcessorBeautyName<processorTypes::PeakTracker>();
template <>
std::string getProcessorBeautyName<processorTypes::MIDIReceiver>();
template <>
std::string getProcessorBeautyName<processorTypes::RemoteChReceiver>();
//-----------------------------------------------------------------------------
template <class _ProcessorType>
std::string getProcessorTooltip() {return "?";}
template <>
std::string getProcessorTooltip<processorTypes::Plugin>();
template <>
std::string getProcessorTooltip<processorTypes::Volume>();
template <>
std::string getProcessorTooltip<processorTypes::Pan>();
template <>
std::string getProcessorTooltip<processorTypes::InStep>();
template <>
std::string getProcessorTooltip<processorTypes::OutStep>();
template <>
std::string getProcessorTooltip<processorTypes::InSwitch>();
template <>
std::string getProcessorTooltip<processorTypes::OutSwitch>();
template <>
std::string getProcessorTooltip<processorTypes::ADSR>();
template <>
std::string getProcessorTooltip<processorTypes::PeakTracker>();
template <>
std::string getProcessorTooltip<processorTypes::MIDIReceiver>();
template <>
std::string getProcessorTooltip<processorTypes::RemoteChReceiver>();
}}} // namespace(s)

#endif /* SAMBAG_FRXPLUGINNODE_H */
