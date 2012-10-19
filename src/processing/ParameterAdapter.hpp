/*
 * ParameterAdapter.hpp
 *
 *  Created on: Thu Oct 18 13:31:50 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PARAMETERADAPTER_H
#define SAMBAG_PARAMETERADAPTER_H

#include <boost/shared_ptr.hpp>
#include "IParameter.hpp"
#include "parameter/parameter.h"
#include <sambag/com/Exception.hpp>

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class ParameterAdapter.
  */
class ParameterAdapter : public IParameter {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ParameterAdapter> Ptr;
	//-------------------------------------------------------------------------
	typedef ::processing::parameter::Parameter Adaptee;
protected:
	//-------------------------------------------------------------------------
	Adaptee::Ptr parameter;
	//-------------------------------------------------------------------------
	ParameterAdapter(){}
public:
	//-------------------------------------------------------------------------
	void setAdaptee(Adaptee::Ptr p) {
		parameter = p;
	}
	//-------------------------------------------------------------------------
	Adaptee::Ptr getAdaptee() const {
		return parameter;
	}
	//-------------------------------------------------------------------------
	static Ptr create(Adaptee::Ptr a = Adaptee::Ptr()) {
		Ptr res(new ParameterAdapter());
		res->setAdaptee(a);
		return res;
	}
	//-------------------------------------------------------------------------
	/**
	 * @see VST-SDK:
	 * Stuff text with the name
	 * ("Time", "Gain", "RoomType", etc...) of parameter index.
	 */
	virtual std::string getName() const {
		SAMBAG_ASSERT(parameter);
		return parameter->getName();
	}
	//-------------------------------------------------------------------------
	/**
	 * @see VST-SDK:
	 * Stuff text with a string representation
	 * ("0.5", "-3", "PLATE", etc...) of the value of parameter index.
	 */
	virtual std::string getDisplay() const  {
		SAMBAG_ASSERT(parameter);
		return parameter->getDisplay();
	}
	//-------------------------------------------------------------------------
	/**
	 * @see VST-SDK:
	 * Stuff label with the units in which parameter index is displayed
	 * (i.e. "sec", "dB", "type", etc...).
	 */
	virtual std::string getLabel() const  {
		SAMBAG_ASSERT(parameter);
		return parameter->getLabel();
	}
	//-------------------------------------------------------------------------
	/**
	 * @see VST-SDK:
	 * Stuff text with the name
	 * ("Time", "Gain", "RoomType", etc...) of parameter index.
	 */
	virtual void setName(const std::string &str)  {
		SAMBAG_ASSERT(parameter);
		parameter->setName(str);
	}
	//-------------------------------------------------------------------------
	/**
	 * @see VST-SDK:
	 * Stuff text with a string representation
	 * ("0.5", "-3", "PLATE", etc...) of the value of parameter index.
	 */
	virtual void setDisplay(const std::string &str) {
		SAMBAG_ASSERT(parameter);
		parameter->setName(str);
	}
	//-------------------------------------------------------------------------
	/**
	 * @see VST-SDK:
	 * Stuff label with the units in which parameter index is displayed
	 * (i.e. "sec", "dB", "type", etc...).
	 */
	virtual void setLabel(const std::string &str)  {
		SAMBAG_ASSERT(parameter);
		parameter->setName(str);
	}
	//-------------------------------------------------------------------------
	virtual ParameterChanged & getEventSender() const {
		SAMBAG_ASSERT(parameter);
		return *(parameter.get());
	}
}; // ParameterAdapter
}} // namespace(s)

#endif /* SAMBAG_PARAMETERADAPTER_H */
