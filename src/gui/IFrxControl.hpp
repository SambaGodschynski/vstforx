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
#include <gui/HandyNamespaces.hpp>
#include <processing/IModelController.hpp>
#include "IViewModelMap.hpp"
#include <gui/components/FrxConnection.hpp>

#define SAMBAG_CREATE_FRXCONTROL_CMD(frxctrl, view, frxcomponent, frxcmdfunction) \
  ((frxctrl).createCtrlCommandFunction(                                           \
    (view),                                                                       \
    (frxcomponent),                                                               \
	boost::bind((frxcmdfunction), &(frxctrl), _1, _2)                             \
  ))

#define SAMBAG_CREATE_FRXCONTROL_CMD1(frxctrl, view, frxcomponent, frxcmdfunction, arg1) \
  ((frxctrl).createCtrlCommandFunction(                                           \
    (view),                                                                       \
    (frxcomponent),                                                               \
	boost::bind((frxcmdfunction), &(frxctrl), _1, _2, (arg1))                     \
  ))

#define SAMBAG_CREATE_FRXCONTROL_CMD2(frxctrl, view, frxcomponent, frxcmdfunction, arg1, arg2) \
  ((frxctrl).createCtrlCommandFunction(                                           \
    (view),                                                                       \
    (frxcomponent),                                                               \
	boost::bind((frxcmdfunction), &(frxctrl), _1, _2, (arg1), (arg2))             \
  ))


namespace frx { namespace gui {
//=============================================================================
/** 
  * @class IFrxControl.
  */
class IFrxControl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	virtual void registerComponent(fgc::FrxCircuidViewPtr view, fgc::FrxComponentPtr c) = 0;
	//-------------------------------------------------------------------------
	/**
	 * holds window pointer until window is closed.
	 * @param the window
	 * @param the windowClassName, if setted only one ptr per class will be stored
	 */
	virtual void addWindow(sdc::WindowPtr win, const std::string &wndClass="") = 0;
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
	/**
	 * creates and adds a knob which is related to another view object to view.
	 * @return new created knob
	 * @param the view
	 * @param the related view object
	 * @param the knob model object
	 */
	virtual fgc::FrxComponentPtr addRelatedKnobToView(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c, frx::processing::IParameter::Ptr par) = 0; 
	//-------------------------------------------------------------------------
	/**
	 * @return new created input node
	 */
	virtual fgc::FrxComponentPtr addProcessorInput(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c) = 0; 
	//-------------------------------------------------------------------------
	/**
	 * @return new created output node
	 */
	virtual fgc::FrxComponentPtr addProcessorOutput(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c) = 0; 
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
	//-------------------------------------------------------------------------
	typedef frx::processing::ParameterCnOpTypeId ParameterCnOpTypeId;
	//-------------------------------------------------------------------------
	typedef frx::processing::ParameterCnOpTypeIds ParameterCnOpTypeIds;
	//-------------------------------------------------------------------------
	virtual void getParameterCnOpTypeIds(fgc::FrxCircuidViewPtr view, 
		ParameterCnOpTypeIds &out) const = 0;
    //-------------------------------------------------------------------------
    typedef size_t OperatorId;
    typedef std::string OperatorName;
    typedef std::pair<OperatorId, OperatorName> Operator;
    /**
     * @return operator names contained by connection
     */
    virtual void getOperators( fgc::FrxCircuidViewPtr view,
        fgc::FrxConnection::Ptr con, std::vector<Operator> &out) = 0;
    //-------------------------------------------------------------------------
    virtual void removeOperator(fgc::FrxCircuidViewPtr view,
        fgc::FrxConnection::Ptr con, OperatorId) = 0;
	//-------------------------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////
	// CtrlCmd's: use it with createCtrlCommandFunction() to create
	// menu ActionEvents
	//-------------------------------------------------------------------------
	virtual void removeComponent(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c) = 0;
	//-------------------------------------------------------------------------
	virtual void showProcessorDetails(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c) = 0;
	//-------------------------------------------------------------------------
	virtual void showConnectionDetails(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c) = 0;
	//-------------------------------------------------------------------------
	virtual void openClosePluginEditor(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c) = 0;
	//-------------------------------------------------------------------------
	virtual void addParamterCnOp(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c, const ParameterCnOpTypeId &id) = 0;
}; // IFrxControl
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
extern IFrxControl & getFrxControl(fgc::FrxCircuidViewPtr view);
//-----------------------------------------------------------------------------
extern boost::tuple<
	frx::processing::IModelController::Ptr,
	IViewModelMap::Ptr
>
getControllerAndMap(fgc::FrxCircuidViewPtr circ);
}} // namespace(s)

#endif /* SAMBAG_IFRXCONTROL_H */
