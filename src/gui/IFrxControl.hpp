/*
 * IFrxControl.hpp
 *
 *  Created on: Mon Oct 15 10:39:08 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IFRXCONTROL_H
#define SAMBAG_IFRXCONTROL_H

#include <sambag/disco/components/Forward.hpp>
#include <sambag/disco/components/events/ActionEvent.hpp>
#include <sambag/disco/components/events/MouseEvent.hpp>
#include "components/Forward.hpp"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include <sambag/com/events/Events.hpp>
#include <processing/IParameter.hpp>
#include <processing/PlugInfo.h>

#define SAMBAG_CREATE_FRXCONTROL_CMD(frxctrl, view, frxcomponent, frxcmdfunction) \
  ((frxctrl).createCtrlCommandFunction(                                           \
    (view),                                                                       \
    (frxcomponent),                                                               \
	boost::bind((frxcmdfunction), &(frxctrl), _1, _2)                                                              \
  ))


namespace frx { namespace gui {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
namespace fgc = frx::gui::components;
//=============================================================================
/** 
  * @class IFrxControl.
  */
class IFrxControl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	virtual void addProcessorToView(fgc::FrxCircuidViewPtr view, 
		fgc::FrxProcessorNodePtr pr) = 0;
	//-------------------------------------------------------------------------
	virtual void addParameterToView(fgc::FrxCircuidViewPtr view, 
		fgc::FrxParameterPtr pr) = 0;
	//-------------------------------------------------------------------------
	typedef boost::function<void(fgc::FrxCircuidViewPtr, 
		fgc::FrxComponentPtr)> CtrlCmd;
	//-------------------------------------------------------------------------
	/**
	 * @return tuple(entry, exit)
	 */
	virtual boost::tuple<fgc::FrxNodePtr, fgc::FrxNodePtr>
	createEntryExtitNodes(fgc::FrxCircuidViewPtr c) = 0;
	//-------------------------------------------------------------------------
	virtual sdc::PopupMenuPtr 
	getCircuidViewPopup(fgc::FrxCircuidViewPtr c) = 0;
	//-------------------------------------------------------------------------
	virtual bool 
	connect(fgc::FrxCircuidViewPtr, fgc::FrxNodePtr from, fgc::FrxNodePtr to) = 0;
	//-------------------------------------------------------------------------
	virtual void 
	handleContextMenuPopup(const sdc::events::MouseEvent &ev) = 0;
	//-------------------------------------------------------------------------
	virtual void addProcesorKnobToView(fgc::FrxCircuidViewWPtr view, 
		fgc::FrxComponentWPtr c, frx::processing::IParameter::WPtr par) = 0; 
	//-------------------------------------------------------------------------
	/**
	 * @return a function object which is able to be executed by
	 * a EventSender<ActionActionEvent> instance via FrxControl.
	 */
	virtual sambag::com::events::EventSender<sdc::events::ActionEvent>::EventFunction
	createCtrlCommandFunction(fgc::FrxCircuidViewPtr view,
		fgc::FrxComponentPtr comp,
		const CtrlCmd &cmdF
	) = 0;
	///////////////////////////////////////////////////////////////////////////
	// CtrlCmd's: use it with createCtrlCommandFunction() to create
	// menu ActionEvents
	//-------------------------------------------------------------------------
	virtual void removeComponent(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c) = 0;
	//-------------------------------------------------------------------------
	virtual void showProcessorDetails(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c) = 0;
}; // IFrxControl
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
extern IFrxControl & getFrxControl(fgc::FrxCircuidViewPtr view);
}} // namespace(s)

#endif /* SAMBAG_IFRXCONTROL_H */
