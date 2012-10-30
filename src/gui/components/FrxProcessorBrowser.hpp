/*
 * FrxProcessorBrowser.hpp
 *
 *  Created on: Tue Oct 23 10:09:40 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPROCESSORBROWSER_H
#define SAMBAG_FRXPROCESSORBROWSER_H

#include <boost/shared_ptr.hpp>
#include "FrxColumnBrowser.hpp"
#include <sambag/disco/components/events/ActionEvent.hpp>
#include <sambag/disco/components/Button.hpp>
#include <sambag/disco/components/Forward.hpp>
namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxProcessorBrowser.
  */
class FrxProcessorBrowser : public FrxColumnBrowser {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxProcessorBrowser> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxProcessorBrowser> WPtr;
	//-------------------------------------------------------------------------
	typedef FrxColumnBrowser Super;
protected:
	//-------------------------------------------------------------------------
	virtual void onClose(void *src, const sdc::events::ActionEvent &ev);
	//-------------------------------------------------------------------------
	virtual void onAdd(void *src, const sdc::events::ActionEvent &ev);
	//-------------------------------------------------------------------------
	FrxProcessorBrowser(sdc::Window::Ptr parent=sdc::Window::Ptr()) :
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
private:
public:
}; // FrxProcessorBrowser
}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSORBROWSER_H */
