/*
 * FrxHover.hpp
 *
 *  Created on: Thu Sep 27 14:56:25 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXHOVER_H
#define SAMBAG_FRXHOVER_H

#include <boost/shared_ptr.hpp>
#include "FrxSelection.hpp"
#include "IFormatter.hpp"

namespace frx { namespace gui { namespace components {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
class FrxCircuidView;
//=============================================================================
/** 
  * @class FrxHover.
  * A Hover implements the "place" and drop functionallity when a new component
  * is being added to view.
  */
class FrxHover : public FrxSelection {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxSelection Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxHover> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxHover(){ 
		setName("FrxHover");
		setVisible(false); 
	}
	//-------------------------------------------------------------------------
	IFormatter::Ptr formatter;
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
public:
	//-------------------------------------------------------------------------
	IFormatter::Ptr getFormatter() const {
		return formatter;
	}
	//-------------------------------------------------------------------------
	void setFormatter(IFormatter::Ptr fomatter);
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(FrxHover)
	//-------------------------------------------------------------------------
	sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const {
		return laf->getUI<FrxHover>();
	}
}; // FrxHover
}}}
#endif /* SAMBAG_FRXHOVER_H */
