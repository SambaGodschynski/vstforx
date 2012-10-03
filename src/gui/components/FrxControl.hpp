/*
 * FrxControl.hpp
 *
 *  Created on: Thu Sep 27 14:49:27 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCONTROL_H
#define SAMBAG_FRXCONTROL_H

#include <sambag/disco/components/Forward.hpp>
#include <sambag/disco/components/events/ActionEvent.hpp>
#include <sambag/disco/components/events/MouseEvent.hpp>
#include <loki/Singleton.h>
#include "Forward.hpp"
#include <list>
#include <string>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace frx { namespace gui { namespace components {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxControl.
  */
class FrxControl {
//=============================================================================
friend struct Loki::CreateUsingNew<FrxControl>;
public:
	//-------------------------------------------------------------------------
	typedef boost::function<void()> CtrlFunc;
	//-------------------------------------------------------------------------
	typedef std::pair<std::string, CtrlFunc> Entry;
	//-------------------------------------------------------------------------
	typedef std::list<Entry> Entries;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<void> AnyWPtr;
protected:
	//-------------------------------------------------------------------------
	/**
	 * called when menu btn action performed.
	 * @param source ptr
	 * @param event
	 * @param command to perform
	 */
	void onMenuAction(void *src, 
			const sdc::events::ActionEvent &ev, 
			const CtrlFunc &cmd);
	//-------------------------------------------------------------------------
	sdc::PopupMenuPtr currPopup;
public:
	//-------------------------------------------------------------------------
	/**
	 * creates Popupmenu for entries.
	 * @param anyPtr for signal tracking
	 * @entries
	 */
	sdc::PopupMenuPtr createPopupMenu(AnyWPtr anyPtr, const Entries &e);
	//-------------------------------------------------------------------------
	sdc::PopupMenuPtr getCircuidViewPopup(FrxCircuidViewPtr c);
	//-------------------------------------------------------------------------
	bool connect(FrxCircuidViewPtr, FrxNodePtr from, FrxNodePtr to);
	//-------------------------------------------------------------------------
	void handleContextMenuPopup(const sdc::events::MouseEvent &ev);
}; // FrxControl
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
extern FrxControl & getFrxControl(FrxCircuidViewPtr view);
}}} // namespace(s)

#endif /* SAMBAG_FRXCONTROL_H */
