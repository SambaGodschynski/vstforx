/*
 * FrxControl.hpp
 *
 *  Created on: Thu Sep 27 14:49:27 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCONTROL_H
#define SAMBAG_FRXCONTROL_H

#include "IFrxControl.hpp"
#include "components/FrxCircuidView.hpp"
#include "components/Forward.hpp"
#include <processing/Forward.hpp>
#include <sambag/com/ArbitraryType.hpp>

#include <boost/tuple/tuple.hpp>
#include <loki/Singleton.h>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
//=============================================================================
/** 
  * @class FrxControl.
  */
class FrxControl : public IFrxControl {
//=============================================================================
friend struct Loki::CreateUsingNew<FrxControl>;
public:
protected:
	//-------------------------------------------------------------------------
	sdc::PopupMenuPtr currPopup;
	//-------------------------------------------------------------------------
	void executeCtrlCommand(void *src,
		const sdc::events::ActionEvent &ev,
		fgc::FrxCircuidViewWPtr v, 
		fgc::FrxComponentWPtr c, 
		CtrlCmd cmd);
public:
	//-------------------------------------------------------------------------
	fgc::FrxComponentPtr _addRelatedKnobToView(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c, frx::processing::IParameter::Ptr par);
	//-------------------------------------------------------------------------
	void registerComponent(fgc::FrxCircuidViewPtr view, fgc::FrxComponentPtr c);
	//-------------------------------------------------------------------------
	FrxControl();
	//-------------------------------------------------------------------------
	virtual ~FrxControl();
	//-------------------------------------------------------------------------
	template <class Archive>
	static void serializeView(Archive &ar, fgc::FrxCircuidViewPtr c);
	//-------------------------------------------------------------------------
	template <class Archive>
	static fgc::FrxCircuidViewPtr deserializeView(Archive &ar);
	//-------------------------------------------------------------------------
	template <class Archive>
	static void serializeViewComponents(Archive &ar, fgc::FrxCircuidViewPtr c);
	//-------------------------------------------------------------------------
	/**
	 * @return tuple(entry, exit)
	 */
	virtual void
    createEntryExtitNodes(fgc::FrxCircuidViewPtr c,
                          NodeList &o_entries, NodeList & o_exits);
	//-------------------------------------------------------------------------
	sdc::PopupMenuPtr getCircuidViewPopup(fgc::FrxCircuidViewPtr c);
	//-------------------------------------------------------------------------
	fgc::FrxConnection::Ptr
    connect(fgc::FrxCircuidViewPtr, fgc::FrxNodePtr from, fgc::FrxNodePtr to);
	//-------------------------------------------------------------------------
	void handleContextMenuPopup(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void addProcessorToView(fgc::FrxCircuidViewPtr view, 
		fgc::FrxProcessorNodePtr pr);
	//-------------------------------------------------------------------------
	virtual void addParameterToView(fgc::FrxCircuidViewPtr view, 
		fgc::FrxParameterPtr pr);
	//-------------------------------------------------------------------------
	virtual void removeComponent(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c);
	//-------------------------------------------------------------------------
	virtual sambag::com::events::EventSender<sdc::events::ActionEvent>::EventFunction
	createCtrlCommandFunction(fgc::FrxCircuidViewPtr view,
		fgc::FrxComponentPtr comp,
		const CtrlCmd &cmdF
	);
	//-------------------------------------------------------------------------
	virtual void showProcessorDetails(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void showConnectionDetails(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void openClosePluginEditor(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c);
	//-------------------------------------------------------------------------
	fgc::FrxComponentPtr
	addRelatedKnobToView(fgc::FrxCircuidViewPtr view, fgc::FrxComponentPtr c,
		frx::processing::IParameter::Ptr par); 
	//-------------------------------------------------------------------------
	virtual void addWindow(sdc::WindowPtr win, const std::string &wndClass="");
	//-------------------------------------------------------------------------
	virtual fgc::FrxComponentPtr addProcessorInput(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c, bool followMouse=true);
	//-------------------------------------------------------------------------
	virtual fgc::FrxComponentPtr addProcessorOutput(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c, bool followMouse=true);
	//-------------------------------------------------------------------------
	virtual void addParamterCnOp(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c, const ParameterCnOpTypeId &id);
	//-------------------------------------------------------------------------
	virtual void getParameterCnOpTypeIds(fgc::FrxCircuidViewPtr view,
		ParameterCnOpTypeIds &out) const;
    //-------------------------------------------------------------------------
    virtual void getOperators(fgc::FrxCircuidViewPtr view,
        fgc::FrxConnection::Ptr con, std::vector<Operator> &out);
    //-------------------------------------------------------------------------
    virtual void removeOperator(fgc::FrxCircuidViewPtr view,
        fgc::FrxConnection::Ptr con,OperatorId);
    //-------------------------------------------------------------------------
    virtual void openSceneBrowser(fgc::FrxCircuidViewPtr view, const std::string &path);
    //-------------------------------------------------------------------------
    virtual void openSetup(fgc::FrxCircuidViewPtr view);
    //-------------------------------------------------------------------------
    virtual void openAbout(fgc::FrxCircuidViewPtr view);
}; // FrxControl
///////////////////////////////////////////////////////////////////////////////	
//-----------------------------------------------------------------------------
template <class Archive>
void FrxControl::serializeView(Archive &ar, fgc::FrxCircuidViewPtr c) 
{
	ar & c;
}
//-----------------------------------------------------------------------------
template <class Archive>
fgc::FrxCircuidViewPtr FrxControl::deserializeView(Archive &ar) 
{
	fgc::FrxCircuidViewPtr res;
	ar & res;
	return res;
}
//-----------------------------------------------------------------------------
template <class Archive>
void FrxControl::serializeViewComponents(Archive &ar, fgc::FrxCircuidViewPtr c) 
{
	c->serializeComponents(ar);
}
}} // namespace(s)

#endif /* SAMBAG_FRXCONTROL_H */
