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
#include <loki/Singleton.h>
#include "Forward.hpp"
#include <list>
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
	typedef std::pair<const char*, CtrlFunc> Entry;
	//-------------------------------------------------------------------------
	typedef std::list<Entry> Entries;
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
private:
public:
	//-------------------------------------------------------------------------
	sdc::PopupMenuPtr getCircuidViewPopup(FrxCircuidViewPtr c);
}; // FrxControl
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
extern FrxControl & getFrxControl();
}}} // namespace(s)

#endif /* SAMBAG_FRXCONTROL_H */
