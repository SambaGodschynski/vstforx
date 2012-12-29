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
#include <sambag/disco/svg/graphicElements/Style.hpp>
#include <gui/HandyNamespaces.hpp>

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
	std::string upper, lower;
	//-------------------------------------------------------------------------
	sd::Point2D distance;
	//-------------------------------------------------------------------------
	sd::Coordinate hGap;
	//-------------------------------------------------------------------------
	sdsg::Style flagStyle;
	//-------------------------------------------------------------------------
	FrxFlag::WPtr _flag;
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
	sce::EventSender<sce::PropertyChanged>::Connection evcn;
	//-------------------------------------------------------------------------
	sce::EventSender<OnRemoving>::Connection rmcn;
	//-------------------------------------------------------------------------
	void installTargetListeners(FrxComponent::Ptr c);
	//-------------------------------------------------------------------------
	void updateText(const std::string &txt);
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
