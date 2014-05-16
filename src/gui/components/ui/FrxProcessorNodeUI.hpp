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
#include <processing/IModelController.hpp>
#include <gui/IViewModelMap.hpp>
#include <gui/IFrxControl.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <gui/IFrxControl.hpp>
#include <map>
#include <boost/assign.hpp>
#include <gui/HandyNamespaces.hpp>
#include <gui/components/FrxCircuidView.hpp>

namespace frx { namespace gui {
namespace components { namespace ui {
	sd::ColorRGBA getProcessorFillColor(sdc::AComponent::Ptr c);
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
namespace {
SAMBAG_PROPERTY_TAG(ProcessorRadiusTag, "Processor.radius");
template <class PrType>
sambag::com::Number getProcessorRadius() {
	return sdcu::getUIPropertyCached<ProcessorRadiusTag>((double)0.);
}
template<class PrType>
std::string getImageName() {
	return std::string( getProcessorName<PrType>() ) + ".image";
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
	//-------------------------------------------------------------------------
	void addHasMultipleInputEntry(sdc::PopupMenuPtr menu, FrxCircuidViewPtr view, 
		FrxComponentPtr c);
	//-------------------------------------------------------------------------
	void addHasMultipleOutputEntry(sdc::PopupMenuPtr menu, FrxCircuidViewPtr view, 
		FrxComponentPtr c);
protected:
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
		if (!hasImage())
			return getProcessorRadius<ConcreteProcessor>();
		sd::Rectangle r = getImage()->getSize();
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
	FrxProcessorNode::Ptr pr = boost::dynamic_pointer_cast<FrxProcessorNode>(c);
	SAMBAG_ASSERT(pr);
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxProcessorNodeUI<CT>::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	FrxProcessorNode::Ptr node = boost::dynamic_pointer_cast<FrxProcessorNode>(c);
	Super::draw(cn, c);
	if (hasImage()) {
		/*cn->setFillColor(c->getBackground());
		cn->arc(node->getPivot(), getImage()->getSize().width()/2. - 5.);
		cn->fill();*/
		drawImage(cn, c);
		return;
	}
	cn->translate(node->getPivot());
	cn->arc(sd::Point2D(0, 0), getCoreRadius(node));
	cn->setFillColor(sd::ColorRGBA());
	cn->fill();
}
//-----------------------------------------------------------------------------
namespace {
struct ChildComponentMouseHandler {
    void mouseClicked(const sdc::events::MouseEvent &ev) {
        sdc::AComponent::Ptr c = ev.getSource();
        FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
        if(!circ) {
            return;
        }
        getFrxControl(circ).handleContextMenuPopup(ev);
    }
};

void __onChildComponentMouse(const sdc::events::MouseEvent &ev) {
	static ChildComponentMouseHandler childComponentMouseHandler;
	enum  {
		Filter = sdc::events::MouseEvent::DISCO_MOUSE_CLICKED
	};
	sdc::events::MouseEventSwitch<Filter>::
		delegate(ev, childComponentMouseHandler);
}

template <class CT>
void installSpecificDefs(sdc::AComponentPtr c)
{
}
template <>
inline void installSpecificDefs<FrxPluginNode>(sdc::AComponentPtr _c)
{
	FrxComponent::Ptr c = boost::dynamic_pointer_cast<FrxComponent>(_c);
	if (!c) {
		return;
	}
	if (c->getComponentCount() == 0) {
		return;
	}
	FrxCircuidView::Ptr view = c->getFirstContainer<FrxCircuidView>();
	sdc::AComponent::Ptr edctrl = c->getComponent(0);
	if (!edctrl || !view) {
		return;
	}
    // add (e) context listner
    edctrl->sce::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
        boost::bind(&__onChildComponentMouse, _2),
        c
    );
	typedef sce::EventSender<sdc::events::ActionEvent> AcSender;
	AcSender *sender = dynamic_cast<AcSender*>( edctrl.get() );
	if (!sender) {
		return;
	}
	IFrxControl &ctrl = getFrxControl(view); 
	sender->addTrackedEventListener (
		SAMBAG_CREATE_FRXCONTROL_CMD(ctrl, view, c, 
		&IFrxControl::openClosePluginEditor),
		c
	);
}
} // namespace(s)
template <class CT>
void FrxProcessorNodeUI<CT>::installDefaults(sdc::AComponentPtr c) {
	setImage(
		sambag::disco::getResourceManager().getImage( getImageName<CT>() )
	);
	installSpecificDefs<CT>(c);
	c->setBackground( getProcessorFillColor(c) );
	Super::installDefaults(c);
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
inline void createSpecificEntries<FrxPluginNode>(sdc::PopupMenuPtr menu, 
	FrxCircuidViewPtr view, FrxComponentPtr c)
{
	sdc::MenuItem::Ptr m = sdc::MenuItem::create();
	m->setText("open/close editor...");
	IFrxControl &ctrl = getFrxControl(view); 
	m->sdc::EventSender<sdc::events::ActionEvent>::addTrackedEventListener (
		SAMBAG_CREATE_FRXCONTROL_CMD(ctrl, view, c, 
		&IFrxControl::openClosePluginEditor),
		c
	);
	menu->add(m);
}
} // namespace(s)
//-----------------------------------------------------------------------------
template <class CT>
void FrxProcessorNodeUI<CT>::addHasMultipleInputEntry(sdc::PopupMenuPtr menu,
	FrxCircuidViewPtr view, FrxComponentPtr c)
{
	sdc::MenuItem::Ptr m = sdc::MenuItem::create();
	m->setText("add input");
	IFrxControl &ctrl = getFrxControl(view); 
	m->sdc::EventSender<sdc::events::ActionEvent>::addTrackedEventListener (
		boost::bind(&IFrxControl::addProcessorInput, &ctrl, view, c, true),
		c
	);
	menu->add(m);
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxProcessorNodeUI<CT>::addHasMultipleOutputEntry(sdc::PopupMenuPtr menu, 
	FrxCircuidViewPtr view, FrxComponentPtr c)
{
	sdc::MenuItem::Ptr m = sdc::MenuItem::create();
	m->setText("add output");
	IFrxControl &ctrl = getFrxControl(view); 
	m->sdc::EventSender<sdc::events::ActionEvent>::addTrackedEventListener (
		boost::bind(&IFrxControl::addProcessorOutput, &ctrl, view, c, true),
		c
	);
	menu->add(m); 
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxProcessorNodeUI<CT>::createPopupmenuEntries(sdc::PopupMenuPtr menu, 
	FrxCircuidViewPtr view, 
	FrxComponentPtr c)
{
	Super::createPopupmenuEntries(menu, view, c);
	// io
	//frx::processing::IModelController::Ptr mCtrl;
	IViewModelMap::Ptr map = getViewModelMap(view);
	
	frx::processing::IProcessor::Ptr processor =
		boost::dynamic_pointer_cast<frx::processing::IProcessor>(
			map->getModelObject(c)
		);
	if (!processor)
		return;
	if (processor->hasMultipleInputs()) {
		addHasMultipleInputEntry(menu, view, c);
	}
	if (processor->hasMultipleOutputs()) {
		addHasMultipleOutputEntry(menu, view, c);
	}
	// browser
	sdc::MenuItem::Ptr m = sdc::MenuItem::create();
	m->setText("show details...");
	IFrxControl &ctrl = getFrxControl(view); 
	m->sdc::EventSender<sdc::events::ActionEvent>::addTrackedEventListener (
		SAMBAG_CREATE_FRXCONTROL_CMD(ctrl, view, c, 
		&IFrxControl::showProcessorDetails),
		c
	);
	menu->add(m);
	createSpecificEntries<CT>(menu, view, c);
}
}}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSORNODEUI_H */
