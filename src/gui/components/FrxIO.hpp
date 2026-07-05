/*
 * FrxIO.hpp
 *
 *  Created on: Mon Aug 27 10:35:21 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXIO_H
#define SAMBAG_FRXIO_H

#include <memory>
#include "FrxNode.hpp"
#include <gui/HandyNamespaces.hpp>
#include <boost/serialization/version.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxIO.
  */
class FrxIO : public FrxNode {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	static const std::string PROPERTY_STATE;
	//-------------------------------------------------------------------------
	static const std::string PROPERTY_DISPLAY_TXT;
	//-------------------------------------------------------------------------
	typedef FrxNode Super;
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<FrxIO> Ptr;
	//-------------------------------------------------------------------------
	enum State {
		None, 
		/**
		 * for ex. on step processors, means: this (state)node is active 
		 */
		Activated 
	};
protected:
	//-------------------------------------------------------------------------
	FrxIO() : states(0) {}
	//-------------------------------------------------------------------------
	unsigned int states;
	//-------------------------------------------------------------------------
	std::string display;
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) { 
		ar & boost::serialization::base_object<Super>(*this);
		ar & states;
		if (version > 0) {
			ar & display;
		}
	} 
public:
	//-------------------------------------------------------------------------
	virtual void setDisplayText(const std::string &txt);
	//-------------------------------------------------------------------------
	virtual const std::string & getDisplayText() const { return display; }
	//-------------------------------------------------------------------------
	virtual bool getState(State state) const;
	//-------------------------------------------------------------------------
	virtual void setState(State state, bool val);
    //-------------------------------------------------------------------------
    static bool getState(State state, int states);
}; // FrxIO
}}} // namespace(s)

BOOST_CLASS_VERSION(frx::gui::components::FrxIO, 1)

#endif /* SAMBAG_FRXIO_H */
