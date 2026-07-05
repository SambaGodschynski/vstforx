/*
 * FrxPacketUI.hpp
 *
 *  Created on: Tue Jun 24 10:11:04 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPACKETUI_H
#define SAMBAG_FRXPACKETUI_H

#include <memory>
#include "FrxNodeUI.hpp"
#include <sambag/disco/components/PopupMenu.hpp>
#include <sambag/disco/IResourceManager.hpp>
#include <processing/IModelController.hpp>
#include <gui/IViewModelMap.hpp>
#include <gui/IFrxControl.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <gui/HandyNamespaces.hpp>
#include <gui/components/FrxCircuidView.hpp>

namespace frx { namespace gui {
namespace components { namespace ui {
//=============================================================================
/** 
  * @class FrxPacketUI.
  */
class FrxPacketUI : public FrxNodeUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxNodeUI Super;
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<FrxPacketUI> Ptr;
	//-------------------------------------------------------------------------
	typedef std::weak_ptr<FrxPacketUI> WPtr;
protected:
	//-------------------------------------------------------------------------
	FrxPacketUI();
	//-------------------------------------------------------------------------
	virtual void createPopupmenuEntries(sdc::PopupMenuPtr menu, 
		FrxCircuidViewPtr view, 
		FrxComponentPtr c);
public:
	//-------------------------------------------------------------------------
	virtual void installDefaults(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void installListeners(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void beginConnecting(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	virtual void connecting(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	virtual void endConnecting(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	virtual sambag::com::Number getCoreRadius(sdc::AComponentPtr c) const;
	//-------------------------------------------------------------------------
	static Ptr create();
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
}; // FrxPacketUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXPACKETUI_H */
