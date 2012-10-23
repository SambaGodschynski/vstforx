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
#include <processing/Forward.hpp>
#include <sambag/com/ArbitraryType.hpp>

namespace frx { namespace gui {
namespace sdc = sambag::disco::components;
namespace sc = sambag::com;
namespace sdcu = sdc::ui;
namespace fgc = frx::gui::components;
namespace gc = gui::components;
namespace pr = frx::processing;
//=============================================================================
/**
 * @class FrxBrowser content.
 */
struct BrowserNode {
//=============================================================================
	enum Type{
		Undefined, 
		AddParameter, 
		AddInputNode, 
		AddOutputNode, 
		PerformAction
	};
	std::string name;
	sc::ArbitraryType::Ptr data;
	Type type;
	BrowserNode(const std::string &name, 
		Type type = Undefined,
		sc::ArbitraryType::Ptr data = sc::ArbitraryType::Ptr()
	) : name(name), type(type),  data(data)
	{
	}
	BrowserNode(const char *name = "") : name(name), type(Undefined) 
	{
	}
	bool operator==(const BrowserNode &n) const { 
		return name==n.name && 
			type == n.type &&
			data == n.data;
	}
};
inline std::ostream & operator <<(std::ostream &os, const BrowserNode &n) {
	os<<n.name;
	return os;
}
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
