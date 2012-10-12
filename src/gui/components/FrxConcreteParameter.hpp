/*
 * FrxConcreteParameter.hpp
 *
 *  Created on: Wed Aug 29 12:30:12 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCONCRETEPARAMETER_H
#define SAMBAG_FRXCONCRETEPARAMETER_H

#include <boost/shared_ptr.hpp>
#include "FrxParameter.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <sambag/disco/components/Knob.hpp>

namespace frx { namespace gui { namespace components {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;

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
protected:
	//-------------------------------------------------------------------------
	FrxConcreteParameter() {
		setName("FrxConcreteParameter");
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
		return boost::shared_dynamic_cast<ThisClass>(Super::getPtr());
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
		}
	};	
} // namespace
typedef FrxConcreteParameter<contollerTypes::StdKnob> FrxStdKnob;
}}} // namespace(s)

#endif /* SAMBAG_FRXCONCRETEPARAMETER_H */
