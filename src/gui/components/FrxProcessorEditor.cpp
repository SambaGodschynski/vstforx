/*
 * FrxProcessorEditor.cpp
 *
 *  Created on: Tue Nov  6 10:31:11 2012
 *      Author: Johannes Unger
 */

#include <boost/bind.hpp>
#include "FrxProcessorEditor.hpp"
#include <com/one4All.h>

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
    if (!ctrl) {
        return;
    }
    try {
        ctrl->open(getPtr());
    } catch(const std::exception &ex) {
        close();
        ::com::osMessageBox("Error:", "opening editor failed: " +
                            std::string(ex.what()), ::com::MSG_ALERT);
        return;
	} catch (...) {
        close();
		::com::osMessageBox("Error:", "opening editor failed: unkown error.",
                            ::com::MSG_ALERT);
        return;
	}
		
}
//-----------------------------------------------------------------------------
void FrxProcessorEditor::onClosingWindow(void *src, const sdc::OnCloseEvent &ev)
{
    if (!ctrl) {
        return;
    }
    try {
        ctrl->close(getPtr());
        ctrl.reset();
    } catch(const std::exception &ex) {
        ::com::osMessageBox("Error:", "closing editor failed: " +
                            std::string(ex.what()), ::com::MSG_ALERT);
        return;
	} catch (...) {
		::com::osMessageBox("Error:", "closing editor failed: unkown error.",
                            ::com::MSG_ALERT);
        return;
	}}
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
