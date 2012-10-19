/*
 * IParameter.hpp
 *
 *  Created on: Sat Oct  6 13:52:58 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IPARAMETER_H
#define SAMBAG_IPARAMETER_H

#include <boost/shared_ptr.hpp>
#include "ModelObject.hpp"
#include <string>

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class IParameter.
  */
class IParameter : public ModelObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IParameter> Ptr;
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
}; // IParameter
}} // namespace(s)

#endif /* SAMBAG_IPARAMETER_H */
