/*
 * FrxConcreteProcessor.hpp
 *
 *  Created on: Mon Aug 20 12:08:05 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPLUGINNODE_H
#define SAMBAG_FRXPLUGINNODE_H

#include <memory>
#include "FrxProcessorNode.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <gui/HandyNamespaces.hpp>
#include <sambag/disco/components/Button.hpp>
#include <loki/Typelist.h>
#include <gui/ViewFactory.hpp>
#include <string>

namespace frx { namespace gui { namespace components {
//-----------------------------------------------------------------------------
template <class Processor>
const char * getProcessorName();
//-----------------------------------------------------------------------------
template <class Processor>
const char * getProcessorBeautyName();
//-----------------------------------------------------------------------------
template <class Processor>
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
	typedef std::shared_ptr<ConcreteProcessor> Ptr;
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
			setName(getName());
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
		return std::dynamic_pointer_cast<ConcreteProcessor>(Super::getPtr());
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
	struct ProcessorTypeBase{
        void init( FrxProcessorNode::Ptr ){}
    };
	struct Plugin : ProcessorTypeBase{ 
		bool _isSynth;
		void init( FrxProcessorNode::Ptr obj );
		Plugin() : _isSynth(false) {}
		void isSynth(bool val) { _isSynth = val; }
		bool isSynth() const { return _isSynth; }
        struct Details {
            static const char * ns;
            static const char * name;
            static const char * beautyName;
            static const char * toolTip;
        };
	};
	struct Volume : ProcessorTypeBase {
        struct Details {
            static const char * ns;
            static const char * name;
            static const char * beautyName;
            static const char * toolTip;
        };
    };
	struct Pan : ProcessorTypeBase {
        struct Details {
            static const char * ns;
            static const char * name;
            static const char * beautyName;
            static const char * toolTip;
        };
    };
	struct InStep : ProcessorTypeBase {
        struct Details {
            static const char * ns;
            static const char * name;
            static const char * beautyName;
            static const char * toolTip;
        };
    };
	struct OutStep : ProcessorTypeBase {
        struct Details {
            static const char * ns;
            static const char * name;
            static const char * beautyName;
            static const char * toolTip;
        };
    };
	struct InSwitch : ProcessorTypeBase {
        struct Details {
            static const char * ns;
            static const char * name;
            static const char * beautyName;
            static const char * toolTip;
        };
    };
	struct OutSwitch : ProcessorTypeBase {
        struct Details {
            static const char * ns;
            static const char * name;
            static const char * beautyName;
            static const char * toolTip;
        };
    };
	struct ADSR : ProcessorTypeBase {
        struct Details {
            static const char * ns;
            static const char * name;
            static const char * beautyName;
            static const char * toolTip;
        };
    };
	struct PeakTracker : ProcessorTypeBase {
        struct Details {
            static const char * ns;
            static const char * name;
            static const char * beautyName;
            static const char * toolTip;
        };
    };
	struct MIDIReceiver : ProcessorTypeBase {
        struct Details {
            static const char * ns;
            static const char * name;
            static const char * beautyName;
            static const char * toolTip;
        };
    };
    struct RemoteChReceiver : ProcessorTypeBase {
        struct Details {
            static const char * ns;
            static const char * name;
            static const char * beautyName;
            static const char * toolTip;
        };
    };
    struct DCTester : ProcessorTypeBase {
        struct Details {
            static const char * ns;
            static const char * name;
            static const char * beautyName;
            static const char * toolTip;
        };
    };
	struct FrqDetector : ProcessorTypeBase {
        struct Details {
            static const char * ns;
            static const char * name;
            static const char * beautyName;
            static const char * toolTip;
        };
    };
} // namespace

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
typedef FrxConcreteProcessor<processorTypes::FrqDetector> FrxFrqDetector;
///////////////////////////////////////////////////////////////////////////////
typedef LOKI_TYPELIST_13(FrxPluginNode,
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
                 FrxDCTester,
                 FrxFrqDetector
        ) FrxProcessorList;
//-----------------------------------------------------------------------------
template <class _ProcessorType>
const char * getProcessorName() {
    return _ProcessorType::Details::name;
}
//-----------------------------------------------------------------------------
template <class _ProcessorType>
const char * getProcessorBeautyName() {
    return _ProcessorType::Details::beautyName;
}
//-----------------------------------------------------------------------------
template <class _ProcessorType>
const char * getProcessorTooltip() {
    return _ProcessorType::Details::toolTip;
}
//-----------------------------------------------------------------------------
template <class _ProcessorType>
const char * getProcessorNamespace() {
    return _ProcessorType::Details::ns;
}
///////////////////////////////////////////////////////////////////////////////
// Register in Factory
// Walk through ProcessorList and register themselve
///////////////////////////////////////////////////////////////////////////////
namespace {
    template <class T>
    inline bool _doRegister() {
        return ViewFactory::instance().register_<T>(
            std::string(getProcessorNamespace<T>()) + "." + getProcessorName<T>(),
            &T::create
        );
    }
    template <>
    bool _doRegister<FrxPluginNode>();
    template <class ProcessorList>
    inline bool registerInFactory() {
        typedef typename ProcessorList::Head T;
        return _doRegister<T>() && registerInFactory<typename ProcessorList::Tail>();
    }
    template <>
    inline bool registerInFactory<Loki::NullType>() { return true; }
    
} // namespace(s)
}}} // namespace(s)

#endif /* SAMBAG_FRXPLUGINNODE_H */
