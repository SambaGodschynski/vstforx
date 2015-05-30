/*
 * FrxSvgComponentUI.hpp
 *
 *  Created on: Tue Aug 17 17:33:20 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FrxSvgComponentUI_H
#define SAMBAG_FrxSvgComponentUI_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/disco/components/ui/SvgComponentUI.hpp>
#include <sambag/disco/components/Forward.hpp>
#include <sambag/disco/svg/Style.hpp>
#include <sambag/disco/components/SvgComponent.hpp>
#include <sambag/disco/Geometry.hpp>
#include <gui/components/Forward.hpp>
#include <sambag/disco/IResourceManager.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxSvgComponentUI.
  */
class FrxSvgComponentUI : public sdcu::SvgComponentUIBase {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef AComponentUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxSvgComponentUI> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxSvgComponentUI> WPtr;
protected:
    //-------------------------------------------------------------------------
    sdc::AContainerWPtr component;
	//-------------------------------------------------------------------------
	void postConstructor(Ptr self);
	//-------------------------------------------------------------------------
	FrxSvgComponentUI();
	//-------------------------------------------------------------------------
	WPtr self;
	//-------------------------------------------------------------------------
	virtual void createPopupmenuEntries(sdc::PopupMenuPtr menu, 
		FrxCircuidViewPtr view, 
		FrxComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void installDefaults(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void installListeners(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	void setTooltipText(sdc::AComponentPtr c);
    //-------------------------------------------------------------------------
    virtual void installImageComponent(sdc::AComponentPtr c);
private:
	//-------------------------------------------------------------------------
	sdc::SvgComponent::Ptr imageComponent;
public:
    //-------------------------------------------------------------------------
    sdc::SvgComponent::Dummy::Ptr getSvgSubComponent(const std::string &id) const;
    //-------------------------------------------------------------------------
    sdc::AContainerPtr getComponent() const;
	//-------------------------------------------------------------------------
	virtual sdc::PopupMenuPtr createPopupmenu(FrxComponentPtr c, 
		FrxCircuidViewPtr view);
	//-------------------------------------------------------------------------
	virtual ~FrxSvgComponentUI();
	//-------------------------------------------------------------------------
	void setImageComponent(sdc::SvgComponent::Ptr x);
	//-------------------------------------------------------------------------
	virtual sdc::SvgComponent::Ptr getSvgComponent() const {
        return imageComponent;
    }
	//-------------------------------------------------------------------------
	Ptr getPtr() const {
		return self.lock();
	}
	//-------------------------------------------------------------------------
	/**
	 * Configures the specified component appropriately for the look and feel.
	 * @param c
	 */
	virtual void installUI(sdc::AComponentPtr c);
    //-------------------------------------------------------------------------
    virtual void uninstallUI(sdc::AComponentPtr c);
}; // FrxSvgComponentUI
}}}} // namespace(s)

#endif /* SAMBAG_FrxSvgComponentUI_H */
