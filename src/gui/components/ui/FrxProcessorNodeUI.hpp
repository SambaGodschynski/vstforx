/*
 * FrxProcessorNodeUI.hpp
 *
 *  Created on: Mon Aug 20 10:46:06 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPROCESSORNODEUI_H
#define SAMBAG_FRXPROCESSORNODEUI_H

#include <boost/shared_ptr.hpp>
#include <gui/components/FrxProcessorNode.hpp>
#include <gui/components/FrxConcreteProcessor.hpp>
#include "FrxNodeUI.hpp"
#include <sambag/disco/components/PopupMenu.hpp>
#include <sambag/disco/IResourceManager.hpp>
#include <gui/IFrxControl.hpp>
#include <map>
#include <boost/assign.hpp>

namespace frx { namespace gui {
namespace components { namespace ui {
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
namespace {
template <class PrType>
sambag::com::Number getProcessorRadius() {
	SAMBAG_PROPERTY_TAG(PropertyTag, "Processor.radius");
	return sdcu::getUIPropertyCached<PropertyTag>((double)0.);
}
typedef std::map<std::string, std::string> ProcessorImageMap;
ProcessorImageMap processorImageMap;
void initProcessorImageMap() {
	using namespace boost::assign;
	processorImageMap = map_list_of
		(getProcessorName<FrxPluginNode::ProcessorType>(), "FrxPluginNode.image");
}
template<class PrType>
std::string getImageName() {
	ProcessorImageMap::const_iterator it = 
		processorImageMap.find(getProcessorName<PrType>());
	if (it==processorImageMap.end())
		return "ProcessorImageMap unknown request.";
	return it->second;
}
} // namespace

//=============================================================================
/** 
  * @class FrxProcessorNodeUI.
  */
template <class _ConcreteProcessor>
class FrxProcessorNodeUI : public FrxNodeUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxNodeUI Super;
	//-------------------------------------------------------------------------
	typedef _ConcreteProcessor ConcreteProcessor;
	//-------------------------------------------------------------------------
	typedef FrxProcessorNodeUI<ConcreteProcessor> ThisClass;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ThisClass> Ptr;
private:
protected:
	//-------------------------------------------------------------------------
	sd::ISurface::Ptr image;
	//-------------------------------------------------------------------------
	FrxProcessorNodeUI();
	//-------------------------------------------------------------------------
	virtual void installDefaults(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void installListeners(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void createPopupmenuEntries(sdc::PopupMenuPtr menu, 
		FrxCircuidViewPtr view, 
		FrxComponentPtr c);
public:
	//-------------------------------------------------------------------------
	virtual void beginConnecting(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	virtual void connecting(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	virtual void endConnecting(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	virtual sambag::com::Number getCoreRadius(sdc::AComponentPtr c) const {
		if (!image)
			return getProcessorRadius<ConcreteProcessor>();
		sd::Rectangle r = image->getSize();
		return r.width() / 2.;
	}
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res(new ThisClass());
		res->postConstructor(res);
		return res;
	}
	//-------------------------------------------------------------------------
	virtual bool contains(sdc::AComponentPtr c, const sd::Point2D &p);
	//-------------------------------------------------------------------------
	/**
	 * Configures the specified component appropriately for the look and feel.
	 * @param c
	 */
	virtual void installUI(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	/**
	 * Paints the specified component appropriately for the look and feel.
	 * @param cn
	 * @param c
	 */
	virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);
}; // FrxProcessorNodeUI
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class CT>
FrxProcessorNodeUI<CT>::FrxProcessorNodeUI() {
}
//-----------------------------------------------------------------------------
template <class CT>
bool FrxProcessorNodeUI<CT>::contains(sdc::AComponentPtr c, const sd::Point2D &p) 
{
	return Super::contains(c, p);
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxProcessorNodeUI<CT>::installUI(sdc::AComponentPtr c) {
	Super::installUI(c);
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxProcessorNodeUI<CT>::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	Super::draw(cn, c);
	if (image) {
		cn->drawSurface(image);
		return;
	}
	FrxProcessorNode::Ptr node = boost::shared_dynamic_cast<FrxProcessorNode>(c);
	cn->translate(node->getPivot());
	cn->arc(sd::Point2D(0, 0), getCoreRadius(node));
	cn->setFillColor(sd::ColorRGBA());
	cn->fill();
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxProcessorNodeUI<CT>::installDefaults(sdc::AComponentPtr c) {
	Super::installDefaults(c);
	if (processorImageMap.empty()) {
		initProcessorImageMap();
	}
	image = sambag::disco::getResourceManager().getImage( getImageName<CT>() );
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxProcessorNodeUI<CT>::installListeners(sdc::AComponentPtr c) {
	Super::installListeners(c);
}
//-----------------------------------------------------------------------------
namespace {
template <class CT>
void createSpecificEntries(sdc::PopupMenuPtr menu, FrxCircuidViewPtr view, 
	FrxComponentPtr c)
{
}
template <>
inline void createSpecificEntries<FrxPluginNode::ProcessorType>(sdc::PopupMenuPtr menu, 
	FrxCircuidViewPtr view, FrxComponentPtr c)
{
	sdc::MenuItem::Ptr m = sdc::MenuItem::create();
	m->setText("open " + c->getName() + " editor...");
	IFrxControl &ctrl = getFrxControl(view); 
	m->EventSender<sdc::events::ActionEvent>::addTrackedEventListener (
		SAMBAG_CREATE_FRXCONTROL_CMD(ctrl, view, c, 
		&IFrxControl::openPluginEditor),
		c
	);
	menu->add(m);
}
} // namespace(s)
//-----------------------------------------------------------------------------
template <class CT>
void FrxProcessorNodeUI<CT>::createPopupmenuEntries(sdc::PopupMenuPtr menu, 
	FrxCircuidViewPtr view, 
	FrxComponentPtr c)
{
	Super::createPopupmenuEntries(menu, view, c);
	sdc::MenuItem::Ptr m = sdc::MenuItem::create();
	m->setText("show " + c->getName() + " details...");
	IFrxControl &ctrl = getFrxControl(view); 
	m->EventSender<sdc::events::ActionEvent>::addTrackedEventListener (
		SAMBAG_CREATE_FRXCONTROL_CMD(ctrl, view, c, 
		&IFrxControl::showProcessorDetails),
		c
	);
	menu->add(m);
	createSpecificEntries<CT>(menu, view, c);
}
}}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSORNODEUI_H */
