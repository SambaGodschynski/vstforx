/*
 * FrxFlagUI.hpp
 *
 *  Created on: Sun Dec 23 14:31:38 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXFLAGUI_H
#define SAMBAG_FRXFLAGUI_H

#include <boost/shared_ptr.hpp>
#include <gui/components/FrxFlag.hpp>
#include "FrxComponentUI.hpp"
#include <sambag/com/events/PropertyChanged.hpp>
#include <sambag/disco/svg/Style.hpp>
#include <gui/HandyNamespaces.hpp>
#include <sambag/disco/IDrawContext.hpp>
#include <sambag/disco/ISurface.hpp>
#include <sambag/disco/components/events/MouseEvent.hpp>

namespace frx { namespace gui { namespace components { namespace ui {
//=============================================================================
/** 
  * @class FrxFlagUI.
  */
class FrxFlagUI : public FrxComponentUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxFlagUI> Ptr;
	//-------------------------------------------------------------------------
	typedef FrxComponentUI Super;
protected:
	//-------------------------------------------------------------------------
	sd::ISurface::Ptr offSf;
	//-------------------------------------------------------------------------
	sd::IDrawContext::Ptr offCn;
	//-------------------------------------------------------------------------
	// missplaced location workaround
	bool firstDraw;
	//-------------------------------------------------------------------------
	sd::Point2D distance;
	//-------------------------------------------------------------------------
	sd::Coordinate hGap;
	//-------------------------------------------------------------------------
	sds::Style flagStyle;
	//-------------------------------------------------------------------------
	FrxFlag::WPtr _flag;
	//-------------------------------------------------------------------------
	sd::IDrawContext::Ptr getOffscreenContext();
	//-------------------------------------------------------------------------
	void postConstructor(Ptr self);
	//-------------------------------------------------------------------------
	virtual void installDefaults(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void installListeners(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	void onRemovingTarget(void *, const OnRemoving &ev);
	//-------------------------------------------------------------------------
	void onTargetPropertyChanged(void *, const sce::PropertyChanged &ev);
	//-------------------------------------------------------------------------
	void onFlagPropertyChanged(void *, const sce::PropertyChanged &ev);
	//-------------------------------------------------------------------------
	void onMouse(void *, const sdce::MouseEvent &ev);
	//-------------------------------------------------------------------------
	sce::EventSender<sce::PropertyChanged>::Connection evcn;
	//-------------------------------------------------------------------------
	sce::EventSender<OnRemoving>::Connection rmcn;
	//-------------------------------------------------------------------------
	void installTargetListeners(FrxComponent::Ptr c);
	//-------------------------------------------------------------------------
	void updateText();
	//-------------------------------------------------------------------------
	void updateBounds(FrxFlag::Ptr flag);
	//-------------------------------------------------------------------------
	void clip(FrxFlag::Ptr target, sd::IDrawContext::Ptr cn) const;
private:
public:
	//-------------------------------------------------------------------------
	static Ptr create(); 
	//-------------------------------------------------------------------------
	/**
	 * Paints the specified component appropriately for the look and feel.
	 * @param cn
	 * @param c
	 */
	virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);
}; // FrxFlagUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXFLAGUI_H */
