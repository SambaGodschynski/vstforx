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
#include <loki/Typelist.h>

namespace frx { namespace gui { namespace components {
//-----------------------------------------------------------------------------
template <class _ProcessorType>
const char * getProcessorName();
//-----------------------------------------------------------------------------
template <class _ProcessorType>
const char * getProcessorBeautyName();
//-----------------------------------------------------------------------------
template <class _ProcessorType>
const char * getProcessorTooltip();
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
			setName(getProcessorBeautyName<ConcreteProcessor>());
			setName(getName()+"_"+sambag::com::toString(instances));
		}
		setUpperFlagText(getName());
		setTooltipText( getProcessorTooltip<ConcreteProcessor>() );
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
    struct DCTester : ProcessorTypeBase{};
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
typedef FrxConcreteProcessor<processorTypes::DCTester> FrxDCTester;
///////////////////////////////////////////////////////////////////////////////
typedef LOKI_TYPELIST_12(FrxPluginNode,
                 FrxVolumeNode,
                 FrxPanNode,
                 FrxInStepNode,
                 FrxOutStepNode,
                 FrxInSwitchNode,
                 FrxOutSwitchNode,
                 FrxADSRNode,
                 FrxPeakTrackerNode,
                 FrxMIDIReceiver,
                 FrxRemoteChReceiver,
                 FrxDCTester
        ) FrxProcessorList;
//-----------------------------------------------------------------------------
namespace __private {
    const char * _getProcessorNameImpl(size_t index);
    template <int TypeIndex>
    inline const char * _getProcessorName() {
        BOOST_STATIC_ASSERT( TypeIndex < Loki::TL::Length<FrxProcessorList>::value );
        return _getProcessorNameImpl(TypeIndex);
    }
    template <>
    inline const char * _getProcessorName<-1>() {
        return "unkonwn processortype";
    }
}
template <class _ProcessorType>
const char * getProcessorName() {
    enum { Index = ::Loki::TL::IndexOf<FrxProcessorList, _ProcessorType>::value };
    return __private::_getProcessorName<Index>();
}
//-----------------------------------------------------------------------------
namespace __private {
    const char * _getProcessorBeautyNameImpl(size_t index);
    template <int TypeIndex>
    inline const char * _getProcessorBeautyName() {
        BOOST_STATIC_ASSERT( TypeIndex < Loki::TL::Length<FrxProcessorList>::value );
        return _getProcessorBeautyNameImpl(TypeIndex);
    }
    template <>
    inline const char * _getProcessorBeautyName<-1>() {
        return "unkonwn processortype";
    }
}
template <class _ProcessorType>
const char * getProcessorBeautyName() {
    enum { Index = ::Loki::TL::IndexOf<FrxProcessorList, _ProcessorType>::value };
    return __private::_getProcessorBeautyName<Index>();
}
//-----------------------------------------------------------------------------
namespace __private {
    const char * _getProcessorTooltipImpl(size_t index);
    template <int TypeIndex>
    inline const char * _getProcessorTooltip() {
        BOOST_STATIC_ASSERT( TypeIndex < Loki::TL::Length<FrxProcessorList>::value );
        return _getProcessorTooltipImpl(TypeIndex);
    }
    template <>
    inline const char * _getProcessorTooltip<-1>() {
        return "?";
    }
}
template <class _ProcessorType>
const char * getProcessorTooltip() {
    enum { Index = ::Loki::TL::IndexOf<FrxProcessorList, _ProcessorType>::value };
    return __private::_getProcessorTooltip<Index>();
}
}}} // namespace(s)

#endif /* SAMBAG_FRXPLUGINNODE_H */
