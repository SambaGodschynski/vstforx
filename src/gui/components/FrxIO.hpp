/*
 * FrxIO.hpp
 *
 *  Created on: Mon Aug 27 10:35:21 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXIO_H
#define SAMBAG_FRXIO_H

#include <boost/shared_ptr.hpp>
#include "FrxNode.hpp"
#include <gui/HandyNamespaces.hpp>
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
	typedef FrxNode Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxIO> Ptr;
	//-------------------------------------------------------------------------
	enum State {
		None, 
		/**
		 * for ex. on step processors, means: this (step)node is active 
		 */
		Activated 
	};
protected:
	//-------------------------------------------------------------------------
	FrxIO() : states(0) {}
	//-------------------------------------------------------------------------
	unsigned int states;
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
	} 
public:
	//-------------------------------------------------------------------------
	virtual bool getState(State state) const;
	//-------------------------------------------------------------------------
	virtual void setState(State state, bool val);
}; // FrxIO
}}} // namespace(s)

#endif /* SAMBAG_FRXIO_H */
