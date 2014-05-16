/*
 * FrxProcessorEditor.hpp
 *
 *  Created on: Tue Nov  6 10:31:11 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPROCESSOREDITOR_H
#define SAMBAG_FRXPROCESSOREDITOR_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/FramedWindow.hpp>
#include "IFrxProcessorEditorCtrl.hpp"
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxProcessorEditor.
  */
class FrxProcessorEditor : public sdc::FramedWindow {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef sdc::FramedWindow Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxProcessorEditor> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxProcessorEditor(sdc::Window::Ptr parent = sdc::Window::Ptr()) :
		 Super(parent)
	{
	}
    //-------------------------------------------------------------------------
	FrxProcessorEditor(sdc::AWindowImpl::Ptr impl, sdc::Window::Ptr parent = sdc::Window::Ptr()) :
		 Super(impl, parent)
	{
	}
 	//-------------------------------------------------------------------------
	IFrxProcessorEditorCtrl::Ptr ctrl;
	//-------------------------------------------------------------------------
	void onOpeningWindow(void *src, const sdc::OnOpenEvent &ev);
	//-------------------------------------------------------------------------
	void onClosingWindow(void *src, const sdc::OnCloseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	virtual void installListener();
private:
public:
	//-------------------------------------------------------------------------
	SAMBAG_STD_WINDOW_CREATOR(FrxProcessorEditor)
	//-------------------------------------------------------------------------
	IFrxProcessorEditorCtrl::Ptr getControl() const;
	//-------------------------------------------------------------------------
	void setControl(IFrxProcessorEditorCtrl::Ptr ctrl);
}; // FrxProcessorEditor
}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSOREDITOR_H */
