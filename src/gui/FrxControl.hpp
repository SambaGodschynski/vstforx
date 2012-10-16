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

namespace frx { namespace gui {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
namespace fgc = frx::gui::components;
namespace gc = gui::components;
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
	/**
	 * called when menu btn action performed.
	 * @param source ptr
	 * @param event
	 * @param command to perform
	 */
	void onMenuAction(void *src,
			const sdc::events::ActionEvent &ev, 
			const CtrlFunc &cmd);
	//-------------------------------------------------------------------------
	sdc::PopupMenuPtr currPopup;
public:
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
	/**
	 * creates Popupmenu for entries.
	 * @param anyPtr for signal tracking
	 * @entries
	 */
	sdc::PopupMenuPtr createPopupMenu(AnyWPtr anyPtr, const Entries &e);
	//-------------------------------------------------------------------------
	sdc::PopupMenuPtr getCircuidViewPopup(fgc::FrxCircuidViewPtr c);
	//-------------------------------------------------------------------------
	bool connect(fgc::FrxCircuidViewPtr, fgc::FrxNodePtr from, fgc::FrxNodePtr to);
	//-------------------------------------------------------------------------
	void handleContextMenuPopup(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void finalizeDeserialization(fgc::FrxComponentPtr c);
}; // FrxControl
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
