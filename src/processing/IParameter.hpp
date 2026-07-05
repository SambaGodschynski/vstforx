/*
 * IParameter.hpp
 *
 *  Created on: Sat Oct  6 13:52:58 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IPARAMETER_H
#define SAMBAG_IPARAMETER_H

#include <memory>
#include "ModelObject.hpp"
#include <string>
#include <com/Events.h>


namespace frx { namespace processing {
//-----------------------------------------------------------------------------
typedef std::string ParameterCnOpTypeId;
typedef std::vector<ParameterCnOpTypeId> ParameterCnOpTypeIds;
//=============================================================================
/** 
  * @class IParameter.
  */
class IParameter : public ModelObject {
//=============================================================================
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
		ar & boost::serialization::base_object<ModelObject> ( *this );
	}
public:
	//-------------------------------------------------------------------------
	virtual bool isHostParameter() const = 0;
	//-------------------------------------------------------------------------
	typedef ModelObject Super;
	//-------------------------------------------------------------------------
	typedef float Number;
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<IParameter> Ptr;
	//-------------------------------------------------------------------------
	typedef std::weak_ptr<IParameter> WPtr;
	//-------------------------------------------------------------------------
	/**
	 * @see VST-SDK:
	 * Stuff text with the name
	 * ("Time", "Gain", "RoomType", etc...) of parameter index.
	 */
	virtual std::string getName() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @see VST-SDK:
	 * Stuff text with a string representation
	 * ("0.5", "-3", "PLATE", etc...) of the value of parameter index.
	 */
	virtual std::string getDisplay() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @see VST-SDK:
	 * Stuff label with the units in which parameter index is displayed
	 * (i.e. "sec", "dB", "type", etc...).
	 */
	virtual std::string getLabel() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @see VST-SDK:
	 * Stuff text with the name
	 * ("Time", "Gain", "RoomType", etc...) of parameter index.
	 */
	virtual void setName(const std::string &) = 0;
	//-------------------------------------------------------------------------
	/**
	 * @see VST-SDK:
	 * Stuff text with a string representation
	 * ("0.5", "-3", "PLATE", etc...) of the value of parameter index.
	 */
	virtual void setDisplay(const std::string &) = 0;
	//-------------------------------------------------------------------------
	/**
	 * @see VST-SDK:
	 * Stuff label with the units in which parameter index is displayed
	 * (i.e. "sec", "dB", "type", etc...).
	 */
	virtual void setLabel(const std::string &) = 0;
	//-------------------------------------------------------------------------
	virtual void setValue(Number value) = 0;
	//-------------------------------------------------------------------------
	virtual Number getValue() const = 0;
	//-------------------------------------------------------------------------
	virtual bool isReadOnly() const = 0;
	//-------------------------------------------------------------------------
	//////////////
	// events
	//-------------------------------------------------------------------------
	typedef ::com::events::ValueChangedSender<float> ParameterChanged;
	virtual ParameterChanged & getEventSender() const = 0;

}; // IParameter
}} // namespace(s)

#endif /* SAMBAG_IPARAMETER_H */
