/*
 * FrxMainBrowser.hpp
 *
 *  Created on: Wed Oct 31 10:30:38 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXMAINBROWSER_H
#define SAMBAG_FRXMAINBROWSER_H

#include <boost/shared_ptr.hpp>
#include "FrxColumnBrowser.hpp"
#include <sambag/disco/components/events/ActionEvent.hpp>
#include <sambag/disco/components/Button.hpp>
#include <sambag/disco/components/Forward.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxMainBrowser.
  * Adds Plugins, Procssors, Knobs, ... .
  */
class FrxMainBrowser : public FrxColumnBrowser {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxMainBrowser> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxMainBrowser> WPtr;
	//-------------------------------------------------------------------------
	typedef FrxColumnBrowser Super;
protected:
	//-------------------------------------------------------------------------
	virtual void onClose(void *src, const sdc::events::ActionEvent &ev);
	//-------------------------------------------------------------------------
	virtual void onAdd(void *src, const sdc::events::ActionEvent &ev);
	//-------------------------------------------------------------------------
	FrxMainBrowser(sdc::Window::Ptr parent=sdc::Window::Ptr()) :
		 FrxColumnBrowser(parent) {}
	//-------------------------------------------------------------------------
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	sdc::ButtonPtr btnAdd, btnClose;
	//-------------------------------------------------------------------------
	virtual void installListeners();
	//-------------------------------------------------------------------------
	virtual void createMainBtns();
public:
	//-------------------------------------------------------------------------
	virtual sdc::ButtonPtr getBtnAdd() const {
		return btnAdd;
	}
	//-------------------------------------------------------------------------
	virtual sdc::ButtonPtr getBtnClose() const {
		return btnClose;
	}
	//-------------------------------------------------------------------------
	static Ptr create( sdc::Window::Ptr parent=sdc::Window::Ptr() );
}; // FrxMainBrowser
}}} // namespace(s)

#endif /* SAMBAG_FRXMAINBROWSER_H */
