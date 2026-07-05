/*
 * FrxSvgPacketUI.hpp
 *
 *  Created on: Tue Jun 24 10:11:04 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FrxSvgPacketUI_H
#define SAMBAG_FrxSvgPacketUI_H

#include <memory>
#include "FrxSvgNodeUI.hpp"
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
  * @class FrxSvgPacketUI.
  */
class FrxSvgPacketUI : public FrxSvgNodeUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxSvgNodeUI Super;
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<FrxSvgPacketUI> Ptr;
	//-------------------------------------------------------------------------
	typedef std::weak_ptr<FrxSvgPacketUI> WPtr;
protected:
	//-------------------------------------------------------------------------
	FrxSvgPacketUI();
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
	static Ptr create();
	//-------------------------------------------------------------------------
	/**
	 * Configures the specified component appropriately for the look and feel.
	 * @param c
	 */
	virtual void installUI(sdc::AComponentPtr c);
    //-------------------------------------------------------------------------
    sdc::PopupMenuPtr createPopupmenu(FrxComponentPtr c, 
		FrxCircuidViewPtr view);


}; // FrxSvgPacketUI
}}}} // namespace(s)

#endif /* SAMBAG_FrxSvgPacketUI_H */
