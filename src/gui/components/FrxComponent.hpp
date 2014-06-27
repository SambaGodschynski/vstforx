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
#include <gui/HandyNamespaces.hpp>
#include <sambag/disco/components/Forward.hpp>
#include <com/Serialization.h>

namespace frx { namespace gui { namespace components {
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
	static const std::string PROPERTY_UPFLAG_TXT;
	//-------------------------------------------------------------------------
	static const std::string PROPERTY_LOFLAG_TXT;
protected:
	//-------------------------------------------------------------------------
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	FrxComponent();
private:
	//-------------------------------------------------------------------------
	std::string uFlagTxt, lFlagTxt, typeId;
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
	void serializeSelfPtr(::com::iArchive &ar, const unsigned int version);
	void serializeSelfPtr(::com::oArchive &ar, const unsigned int version);
	//-------------------------------------------------------------------------
	void save(::com::oArchive &ar, const unsigned int version) const;
	//-------------------------------------------------------------------------
	void load(::com::iArchive &ar, const unsigned int version);
	//-------------------------------------------------------------------------
	void serialize(::com::iArchive &ar, const unsigned int version);
	void serialize(::com::oArchive &ar, const unsigned int version);
public:
    //-------------------------------------------------------------------------
    /**
     * @brief set type id. no to call by client
     */
    void __setTypeId_(const std::string &id);
    //-------------------------------------------------------------------------
    /**
     * @return type id, such as frx.gui.internal.XY
     */
    const std::string & getTypeId() const {
        return typeId;
    }
    //-------------------------------------------------------------------------
    Ptr getPtr() const {
        return boost::dynamic_pointer_cast<FrxComponent>(self.lock());
    }
    //-------------------------------------------------------------------------
	virtual ~FrxComponent();
	//-------------------------------------------------------------------------
	/**
	 * Overridden for performance reasons.
	 * See the <a href="#override">Implementation Note</a>
	 * for more information.
	 */
	virtual void validate() {}
	//-------------------------------------------------------------------------
	/**
	 * Overridden for performance reasons.
	 * See the <a href="#override">Implementation Note</a>
	 * for more information.
	 *
	 * @since 1.5
	 */
	virtual void invalidate() {}
	//-------------------------------------------------------------------------
	/**
	 * Overridden for performance reasons.
	 * See the <a href="#override">Implementation Note</a>
	 * for more information.
	 *
	 * @since 1.5
	 */
	//void redraw() {}
	//-------------------------------------------------------------------------
	/**
	 * Overridden for performance reasons.
	 * See the <a href="#override">Implementation Note</a>
	 * for more information.
	 */
	virtual void revalidate() {}
	//-------------------------------------------------------------------------
	/**
	 * Overridden for performance reasons.
	 * See the <a href="#override">Implementation Note</a>
	 * for more information.
	 */
	//void redraw(const Rectangle &r) {}
	//-------------------------------------------------------------------------
	virtual void setUpperFlagText(const std::string &txt);
	//-------------------------------------------------------------------------
	virtual void setLowerFlagText(const std::string &txt);
	//-------------------------------------------------------------------------
	/**
	 * @return representing string for @seeFrxFlag component.
	 */
	virtual const std::string & getUpperFlagText() const { 
		return uFlagTxt; 
	}
	//-------------------------------------------------------------------------
	/**
	 * @return representing string for @seeFrxFlag component.
	 */
	virtual const std::string & getLowerFlagText() const { 
		return lFlagTxt; 
	}
	//-------------------------------------------------------------------------
	virtual void setBounds(const sd::Rectangle &b);
	//-------------------------------------------------------------------------
	sd::Point2D getPivot() const; 
	//-------------------------------------------------------------------------
	virtual std::string getObjectName() const {
		return AComponent::getName();
	}
    //-------------------------------------------------------------------------
    virtual void setName (const std::string &name);
    
}; // FrxComponent
}}} // namespace(s)

BOOST_CLASS_VERSION(frx::gui::components::FrxComponent, 2);

#endif /* SAMBAG_FRXCOMPONENT_H */
