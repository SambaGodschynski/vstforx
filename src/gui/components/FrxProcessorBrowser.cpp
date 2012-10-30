/*
 * FrxProcessorBrowser.cpp
 *
 *  Created on: Tue Oct 23 10:09:40 2012
 *      Author: Johannes Unger
 */

#include "FrxProcessorBrowser.hpp"


namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxProcessorBrowser
//=============================================================================
//-----------------------------------------------------------------------------
void FrxProcessorBrowser::onClose(void *src, const sdc::events::ActionEvent &ev)
{
	close();
}
//-----------------------------------------------------------------------------
void FrxProcessorBrowser::onAdd(void *src, const sdc::events::ActionEvent &ev)
{

}
//-----------------------------------------------------------------------------
void FrxProcessorBrowser::installListeners() {
	sdc::Button::Ptr btn = getBtnAdd();
	if (btn) {
		btn->EventSender<sdc::events::ActionEvent>::addTrackedEventListener(
			boost::bind(&FrxProcessorBrowser::onAdd, this, _1, _2),
			getPtr()
		);
	}
	btn = getBtnClose();
	if (btn) {
		btn->EventSender<sdc::events::ActionEvent>::addTrackedEventListener(
			boost::bind(&FrxProcessorBrowser::onClose, this, _1, _2),
			getPtr()
		);
	}
}
//-------------------------------------------------------------------------
void FrxProcessorBrowser::createMainBtns() {
	btnAdd = sdc::Button::create();
	btnAdd->setText("add");
	getButtonPane()->add(btnAdd);
	
	btnClose = sdc::Button::create();
	btnClose->setText("close");
	getButtonPane()->add(btnClose);
}
//-----------------------------------------------------------------------------
void FrxProcessorBrowser::postConstructor() {
	Super::postConstructor();
	createMainBtns();
	installListeners();
}
//-----------------------------------------------------------------------------
FrxProcessorBrowser::Ptr FrxProcessorBrowser::create(sdc::Window::Ptr parent) {
	Ptr res( new FrxProcessorBrowser(parent) );
	res->self = res;
	res->postConstructor();
	res->initWindow();
	return res;
}
}}} // namespace(s)
