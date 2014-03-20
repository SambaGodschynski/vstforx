/*
 * FrxConcreteParameter.hpp
 *
 *  Created on: Wed Aug 29 12:30:12 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCONCRETEPARAMETER_H
#define SAMBAG_FRXCONCRETEPARAMETER_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
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
	typedef boost::shared_ptr<FrxConcreteParameter> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxConcreteParameter> WPtr;
protected:
	//-------------------------------------------------------------------------
	FrxConcreteParameter() {
		instances++;
        __setTypeId_(getParameterType<_ControllerType>());
	}
	//-------------------------------------------------------------------------
	virtual void postConstructor() {
		ControllerType::init( getPtr() );
		if (getName() == "") {
			setName(getName()+"_"+sambag::com::toString(instances));
		}
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
		return boost::dynamic_pointer_cast<ThisClass>(Super::getPtr());
	}
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res( new ThisClass() );
		res->self = res;
		res->postConstructor();
		return res;
	}
}; // FrxConcreteParameter
//-----------------------------------------------------------------------------
template <class C>
int FrxConcreteParameter<C>::instances = 0;
//=============================================================================
// Types
//=============================================================================
namespace contollerTypes {
	struct ControllerTypeBase{ void init( FrxParameter::Ptr ){} };
	struct StdKnob : ControllerTypeBase {
		typedef sdc::Knob::Model Model;
		void init( FrxParameter::Ptr obj ){
			obj->setName("Knob");
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
