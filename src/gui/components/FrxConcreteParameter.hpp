/*
 * FrxConcreteParameter.hpp
 *
 *  Created on: Wed Aug 29 12:30:12 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCONCRETEPARAMETER_H
#define SAMBAG_FRXCONCRETEPARAMETER_H

#include <memory>
#include "FrxParameter.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <sambag/disco/components/Knob.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
template <class ParameterType>
std::string getParameterType();
//=============================================================================
/** 
  * @class FrxConcreteParameter.
  */
template <class _ControllerType>
class FrxConcreteParameter : 
	public FrxParameter, 
	public _ControllerType 
{
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef FrxParameter Super;
    //-------------------------------------------------------------------------
    typedef _ControllerType ControllerType;
    //-------------------------------------------------------------------------
    typedef std::shared_ptr<FrxConcreteParameter> Ptr;
    //-------------------------------------------------------------------------
    typedef std::weak_ptr<FrxConcreteParameter> WPtr;
protected:
	//-------------------------------------------------------------------------
	FrxConcreteParameter() {
        __setTypeId_(getParameterType<_ControllerType>());
	}
	//-------------------------------------------------------------------------
	virtual void postConstructor() {
		ControllerType::init( getPtr() );
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
	//-------------------------------------------------------------------------
	typedef FrxConcreteParameter<ControllerType> ThisClass;
public:
	//-------------------------------------------------------------------------
	sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const
	{
		return laf->getUI<ThisClass>();
	}
	//-------------------------------------------------------------------------
	Ptr getPtr() const {
		return std::dynamic_pointer_cast<ThisClass>(Super::getPtr());
	}
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res( new ThisClass() );
		res->self = res;
		res->postConstructor();
		return res;
	}
}; // FrxConcreteParameter
//=============================================================================
// Types
//=============================================================================
namespace contollerTypes {
	struct ControllerTypeBase{ void init( FrxParameter::Ptr ){} };
	struct StdKnob : ControllerTypeBase {
		typedef sdc::Knob::Model Model;
		void init( FrxParameter::Ptr obj ){
			sdc::Knob::Ptr knob(sdc::Knob::create());
			knob->setMinimum(0.);
			knob->setMaximum(1.);
			obj->setEncapsulatedCtrl(knob);
			obj->setRangeModel(knob);
		}
	};	
} // namespace
typedef FrxConcreteParameter<contollerTypes::StdKnob> FrxStdKnob;

template <class ParameterType>
std::string getParameterType() {return "unkonwn connection type";}
template <>
inline std::string getParameterType<contollerTypes::StdKnob>() {return "frx.gui.parameter.StdKnob";}

}}} // namespace(s)

#endif /* SAMBAG_FRXCONCRETEPARAMETER_H */
