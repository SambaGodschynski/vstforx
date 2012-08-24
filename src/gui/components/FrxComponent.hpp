/*
 * FrxComponent.hpp
 *
 *  Created on: 17. Aug 2012
 *      Author: sambag
 */

#ifndef SAMBAG_FRXCOMPONENT_H
#define SAMBAG_FRXCOMPONENT_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/AContainer.hpp>
#include <sambag/disco/Geometry.hpp>

namespace frx { namespace gui { namespace components {
namespace sd = sambag::disco;
namespace sdc = sd::components;
//=============================================================================
/** 
  * @class FrxComponent.
  */
class FrxComponent : public sdc::AContainer {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef sdc::AContainer Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxComponent> Ptr;
	//-------------------------------------------------------------------------
	virtual sdc::ui::AComponentUIPtr 
	createComponentUI(sdc::ui::ALookAndFeelPtr laf) const;
protected:
	//-------------------------------------------------------------------------
	FrxComponent();
private:
public:
	//-------------------------------------------------------------------------
	sd::Point2D getPivot() const; 
}; // FrxComponent
}}} // namespace(s)

#endif /* SAMBAG_FRXCOMPONENT_H */
