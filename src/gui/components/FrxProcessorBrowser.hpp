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
template <class T>
class FrxProcessorBrowser : public FrxColumnBrowser<T> {
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
	virtual void onCancel(void *src, const sdc::events::ActionEvent &ev);
	//-------------------------------------------------------------------------
	virtual void onOk(void *src, const sdc::events::ActionEvent &ev);
	//-------------------------------------------------------------------------
	FrxProcessorBrowser(sdc::Window::Ptr parent=sdc::Window::Ptr()) :
		 FrxColumnBrowser(parent) {}
	//-------------------------------------------------------------------------
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	sdc::ButtonPtr btnOk, btnCancel;
public:
	//-------------------------------------------------------------------------
	sdc::ButtonPtr getBtnOk() const {
		return btnOk;
	}
	//-------------------------------------------------------------------------
	sdc::ButtonPtr getBtnCancel() const {
		return btnCancel;
	}
	//-------------------------------------------------------------------------
	static Ptr create( sdc::Window::Ptr parent=sdc::Window::Ptr() ) {
		Ptr res( new FrxProcessorBrowser(parent) );
		res->self = res;
		res->postConstructor();
		res->initWindow();
		return res;
	}
private:
public:
}; // FrxProcessorBrowser
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class T>
void FrxProcessorBrowser<T>::onCancel(void *src, const sdc::events::ActionEvent &ev)
{
	close();
}
//-----------------------------------------------------------------------------
template <class T>
void FrxProcessorBrowser<T>::onOk(void *src, const sdc::events::ActionEvent &ev)
{
	close();
}
//-----------------------------------------------------------------------------
template <class T>
void FrxProcessorBrowser<T>::postConstructor() {
	Super::postConstructor();
	btnOk = sdc::Button::create();
	btnOk->setText("ok");
	btnOk->EventSender<sdc::events::ActionEvent>::addTrackedEventListener(
		boost::bind(&FrxProcessorBrowser::onOk, this, _1, _2),
		getPtr()
	);
	getButtonPane()->add(btnOk);
	
	btnCancel = sdc::Button::create();
	btnCancel->setText("cancel");
	btnCancel->EventSender<sdc::events::ActionEvent>::addTrackedEventListener(
		boost::bind(&FrxProcessorBrowser::onCancel, this, _1, _2),
		getPtr()
	);
	getButtonPane()->add(btnCancel);
}
}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSORBROWSER_H */
