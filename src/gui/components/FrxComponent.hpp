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
#include <boost/serialization/access.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/weak_ptr.hpp>
#include <boost/serialization/split_member.hpp>
#include <sambag/disco/GeometrySerialization.hpp>

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
	typedef boost::weak_ptr<FrxComponent> WPtr;
	//-------------------------------------------------------------------------
	virtual sdc::ui::AComponentUIPtr 
	createComponentUI(sdc::ui::ALookAndFeelPtr laf) const;
	//-------------------------------------------------------------------------
	static const std::string PROPERTY_FLAG_TXT;
protected:
	//-------------------------------------------------------------------------
	FrxComponent();
private:
	//-------------------------------------------------------------------------
	std::string flagTxt;
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	WPtr tmpSelf;
	//-------------------------------------------------------------------------
	/**
     * we can't serialize "self" directly because it is a AComponent member,
	 * so we need this "trick".
	 */
	template <typename Archive> 
	void serializeSelfPtr(Archive &ar, const unsigned int version) {
		if (Archive::is_saving::value) {
			tmpSelf = boost::shared_dynamic_cast<FrxComponent>(self.lock());
		}
		ar & boost::serialization::base_object<ViewObject>(*this); 
		ar & tmpSelf;
		ar & flagTxt;
		std::string name = getName();
		ar & name;
		if (Archive::is_loading::value) {
			self = tmpSelf;
			postConstructor();
			setName(name);
		}
	}
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void save(Archive &ar, const unsigned int version) const {
		const sd::Rectangle &bounds = getBounds();
		ar << bounds;
	}
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void load(Archive &ar, const unsigned int version) {
		sd::Rectangle bounds;
		ar >> bounds;
		setBounds(bounds);
	}
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
		serializeSelfPtr(ar, version);
		boost::serialization::split_member(ar, *this, version);
	}
public:
	//-------------------------------------------------------------------------
	/**
	 * @return representing string for @seeFrxFlag component.
	 */
	virtual const std::string & getFlagText() const { return flagTxt; }
	//-------------------------------------------------------------------------
	/**
	 * set representing string for @seeFrxFlag component.
	 * upper and lower flag text is seperated by '/':
	 * upperflagtext/lowerflagtext
	 */
	virtual void setFlagText(const std::string &txt);
	//-------------------------------------------------------------------------
	virtual void setBounds(const sd::Rectangle &b);
	//-------------------------------------------------------------------------
	sd::Point2D getPivot() const; 
	//-------------------------------------------------------------------------
	virtual std::string getObjectName() const {
		return AComponent::getName();
	}
}; // FrxComponent
}}} // namespace(s)
#endif /* SAMBAG_FRXCOMPONENT_H */
