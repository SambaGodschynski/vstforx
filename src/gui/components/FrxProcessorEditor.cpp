/*
 * FrxProcessorEditor.cpp
 *
 *  Created on: Tue Nov  6 10:31:11 2012
 *      Author: Johannes Unger
 */

#include "FrxProcessorEditor.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxProcessorEditor
//=============================================================================
//-----------------------------------------------------------------------------
IFrxProcessorEditorCtrl::Ptr FrxProcessorEditor::getControl() const {
	return ctrl;
}
//-----------------------------------------------------------------------------
void FrxProcessorEditor::setControl(IFrxProcessorEditorCtrl::Ptr ctrl) {
	this->ctrl = ctrl;
}
//-----------------------------------------------------------------------------
void FrxProcessorEditor::onOpeningWindow(void *src, const sdc::OnOpenEvent &ev)
{
	if (ctrl)
		ctrl->open(getPtr());
}
//-----------------------------------------------------------------------------
void FrxProcessorEditor::onClosingWindow(void *src, const sdc::OnCloseEvent &ev)
{
	if (ctrl)
		ctrl->close(getPtr());
}
//-----------------------------------------------------------------------------
void FrxProcessorEditor::installListener() {
	addTrackedOnOpenEventListener(
		boost::bind(&FrxProcessorEditor::onOpeningWindow, this, _1, _2),
		getPtr()
	);
	addTrackedOnCloseEventListener(
		boost::bind(&FrxProcessorEditor::onClosingWindow, this, _1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
void FrxProcessorEditor::postConstructor() {
	installListener();
}
}}} // namespace(s)
