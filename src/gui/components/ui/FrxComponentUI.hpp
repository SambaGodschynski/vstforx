/*
 * FrxComponentUI.hpp
 *
 *  Created on: Tue Aug 17 17:33:20 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCOMPONENTUI_H
#define SAMBAG_FRXCOMPONENTUI_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/disco/components/ui/AComponentUI.hpp>
#include <sambag/disco/components/Forward.hpp>
#include <sambag/disco/svg/Style.hpp>
#include <sambag/disco/Geometry.hpp>
#include <gui/components/Forward.hpp>
#include <sambag/disco/IResourceManager.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxComponentUI.
  */
class FrxComponentUI : public sdcu::AComponentUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef AComponentUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxComponentUI> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxComponentUI> WPtr;
protected:
	//-------------------------------------------------------------------------
	void postConstructor(Ptr self);
	//-------------------------------------------------------------------------
	FrxComponentUI();
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
private:
	//-------------------------------------------------------------------------
	sd::ISurface::Ptr image;
public:
	//-------------------------------------------------------------------------
	virtual sdc::PopupMenuPtr createPopupmenu(FrxComponentPtr c, 
		FrxCircuidViewPtr view);
	//-------------------------------------------------------------------------
	virtual ~FrxComponentUI();
	//-------------------------------------------------------------------------
	void setImage(sd::ISurface::Ptr);
	//-------------------------------------------------------------------------
	sd::ISurface::Ptr getImage() const {
		return image;
	}
	//-------------------------------------------------------------------------
	bool hasImage() const {
		return image.get() != NULL;
	}
	//-------------------------------------------------------------------------
	virtual void drawImage(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	Ptr getPtr() const {
		return self.lock();
	}
	//-------------------------------------------------------------------------
	virtual bool contains(sdc::AComponentPtr c, const sd::Point2D &p);
	//-------------------------------------------------------------------------
	/**
	 * Configures the specified component appropriately for the look and feel.
	 * @param c
	 */
	virtual void installUI(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	/**
	 * Paints the specified component appropriately for the look and feel.
	 * @param cn
	 * @param c
	 */
	virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);
}; // FrxComponentUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXCOMPONENTUI_H */
