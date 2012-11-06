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

namespace frx { namespace gui { namespace components {
namespace sce = sambag::com::events;
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
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
