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
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <loki/Singleton.h>
#include <processing/IModelController.hpp>

namespace frx { namespace gui {
namespace sdc = sambag::disco::components;
namespace sc = sambag::com;
namespace sdcu = sdc::ui;
namespace fgc = frx::gui::components;
namespace gc = gui::components;
namespace pr = frx::processing;
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
	FrxControl();
	//-------------------------------------------------------------------------
	virtual ~FrxControl();
	//-------------------------------------------------------------------------
	template <class Archive>
	static void serializeView(Archive &ar, gc::FrxCircuidViewPtr c);
	//-------------------------------------------------------------------------
	template <class Archive>
	static gc::FrxCircuidViewPtr deserializeView(Archive &ar);
	//-------------------------------------------------------------------------
	/**
	 * @return tuple(entry, exit)
	 */
	boost::tuple<fgc::FrxNodePtr, fgc::FrxNodePtr>
	createEntryExtitNodes(fgc::FrxCircuidViewPtr c);
	//-------------------------------------------------------------------------
	sdc::PopupMenuPtr getCircuidViewPopup(fgc::FrxCircuidViewPtr c);
	//-------------------------------------------------------------------------
	bool connect(fgc::FrxCircuidViewPtr, fgc::FrxNodePtr from, fgc::FrxNodePtr to);
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
	virtual void openPluginEditor(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c);
	//-------------------------------------------------------------------------
	fgc::FrxComponentPtr addRelatedKnobToView(gc::FrxCircuidViewPtr view, gc::FrxComponentPtr c,
	frx::processing::IParameter::Ptr par); 
	//-------------------------------------------------------------------------
	virtual void addWindow(sdc::WindowPtr win, const std::string &wndClass="");
	//-------------------------------------------------------------------------
	virtual fgc::FrxComponentPtr addProcessorInput(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c); 
	//-------------------------------------------------------------------------
	virtual fgc::FrxComponentPtr addProcessorOutput(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c); 
	//-------------------------------------------------------------------------
	virtual void addParamterCnOp(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c, const ParameterCnOpTypeId &id);
	//-------------------------------------------------------------------------
	virtual void getParameterCnOpTypeIds(fgc::FrxCircuidViewPtr view, 
		ParameterCnOpTypeIds &out) const;
}; // FrxControl
extern boost::tuple<
	frx::processing::IModelController::Ptr,
	IViewModelMap::Ptr
>
getControllerAndMap(gc::FrxCircuidViewPtr circ);
///////////////////////////////////////////////////////////////////////////////	
//-----------------------------------------------------------------------------
template <class Archive>
void FrxControl::serializeView(Archive &ar, gc::FrxCircuidViewPtr c) 
{
	ar & c;
}
//-----------------------------------------------------------------------------
template <class Archive>
gc::FrxCircuidViewPtr FrxControl::deserializeView(Archive &ar) 
{
	gc::FrxCircuidViewPtr res;
	ar & res;
	return res;
}
}} // namespace(s)

#endif /* SAMBAG_FRXCONTROL_H */
