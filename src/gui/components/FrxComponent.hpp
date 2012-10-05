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
#include <sambag/com/events/Events.hpp>
#include <gui/ViewObject.hpp>
#include "Forward.hpp"

namespace frx { namespace gui { namespace components {
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sce = sambag::com::events;
//=============================================================================
/** 
  * @class OnRemoving.
  */
struct OnRemoving {
//=============================================================================
	FrxCircuidViewPtr view;
	OnRemoving(FrxCircuidViewPtr view) : view(view) {}
};
//=============================================================================
/** 
  * @class FrxComponent.
  */
class FrxComponent : public sdc::AContainer, public ViewObject,
	public sce::EventSender<OnRemoving>
{
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
	virtual void setBounds(const sd::Rectangle &b);
	//-------------------------------------------------------------------------
	sd::Point2D getPivot() const; 
}; // FrxComponent
}}} // namespace(s)

#endif /* SAMBAG_FRXCOMPONENT_H */
