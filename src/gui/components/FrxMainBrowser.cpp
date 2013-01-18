/*
 * FrxMainBrowser.cpp
 *
 *  Created on: Wed Oct 31 10:30:38 2012
 *      Author: Johannes Unger
 */

#include "FrxMainBrowser.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxMainBrowser
//=============================================================================
//-----------------------------------------------------------------------------
void FrxMainBrowser::onClose(void *src, const sdc::events::ActionEvent &ev)
{
	close();
}
//-----------------------------------------------------------------------------
void FrxMainBrowser::onAdd(void *src, const sdc::events::ActionEvent &ev)
{

}
//-----------------------------------------------------------------------------
void FrxMainBrowser::installListeners() {
	sdc::Button::Ptr btn = getBtnAdd();
	if (btn) {
		btn->EventSender<sdc::events::ActionEvent>::addTrackedEventListener(
			boost::bind(&FrxMainBrowser::onAdd, this, _1, _2),
			getPtr()
		);
	}
	btn = getBtnClose();
	if (btn) {
		btn->EventSender<sdc::events::ActionEvent>::addTrackedEventListener(
			boost::bind(&FrxMainBrowser::onClose, this, _1, _2),
			getPtr()
		);
	}
}
//-----------------------------------------------------------------------------
void FrxMainBrowser::createMainBtns() {
	btnAdd = sdc::Button::create();
	btnAdd->setText("add");
	getButtonPane()->add(btnAdd);
	btnAdd->setEnabled(false);
	
	btnClose = sdc::Button::create();
	btnClose->setText("close");
	getButtonPane()->add(btnClose);
}
//-----------------------------------------------------------------------------
void FrxMainBrowser::postConstructor() {
	Super::postConstructor();
	createMainBtns();
	installListeners();
}
//-----------------------------------------------------------------------------
FrxMainBrowser::Ptr FrxMainBrowser::create(sdc::Window::Ptr parent) {
	Ptr res( new FrxMainBrowser(parent) );
	res->self = res;
	res->postConstructor();
	res->initWindow();
	return res;
}
}}} // namespace(s)
