/*
 * FrxCircuidView.cpp
 *
 *  Created on: Mon Aug 20 12:12:59 2012
 *      Author: Johannes Unger
 */
#include "FrxCircuidView.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <gui/IFrxControl.hpp>
#include "FrxComponent.hpp"
#include <list>
#include <algorithm>
#include <OS_Specific/OS_com.h>
#include <sambag/disco/IPattern.hpp>
#include <sambag/disco/IDiscoFactory.hpp>
#include <sambag/disco/IResourceManager.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <sambag/disco/components/Label.hpp>
#include <sambag/disco/components/Window.hpp>
#include <sambag/disco/components/Panel.hpp>
#include <sambag/disco/components/BorderLayout.hpp>
#include <sambag/disco/components/FlowLayout.hpp>
#include <sambag/math/Matrix.hpp>
#include <sambag/disco/svg/graphicElements/Style.hpp>


namespace frx { namespace gui { namespace components {
namespace {
//-----------------------------------------------------------------------------
const float FRX_MAX_VIEW = 10000.;
class BgPane : public sdc::Panel {
public:
	typedef boost::shared_ptr<BgPane> Ptr;
	typedef sdc::Panel Super;
protected:
	BgPane(){}
	sd::IPattern::Ptr pat;
	virtual void postConstructor();
public:
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(BgPane)
	virtual void drawComponent(sd::IDrawContext::Ptr cn);
};
//-----------------------------------------------------------------------------
class StatusLabel : public sdc::Label {
public:
	typedef boost::shared_ptr<StatusLabel> Ptr;
	typedef sdc::Label Super;
protected:
	StatusLabel(){}
public:
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(StatusLabel)
	virtual sd::Dimension getPreferredSize() {
		sd::Dimension sz = Super::getMinimumSize();
		sz.height( sz.height() + 10. );
		return sz; 
	}
};
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
void BgPane::drawComponent(sd::IDrawContext::Ptr cn) {
	if (!pat) {
		Super::drawComponent(cn);
		return;
	}
	cn->setFillPattern(pat);
	cn->rect(sd::Rectangle(0, 0, getWidth(), getHeight()));
	cn->fill();
}
//-----------------------------------------------------------------------------
void BgPane::postConstructor() {
	sd::ISurface::Ptr fillImg = 
		sd::getResourceManager().getImage("FrxCircuidView.image");
	if (!fillImg)
		return;
	pat = sd::getDiscoFactory()->createSurfacePattern(fillImg);
	if (!pat)
		return;
	sdc::ui::UIManager &ui = sdc::ui::getUIManager();
	sambag::math::Matrix m = IDENTITY_MATRIX;
	ui.getProperty("FrxCircuidView.bgTransfomation", m);
	sd::IPattern::Extend e = sd::IPattern::DISCO_EXTEND_REPEAT;
	ui.getProperty("FrxCircuidView.bgExtend", e);
	pat->setMatrix(m);
	pat->setExtendType(e);
}
}// namespace(s)
//=============================================================================
//  Class FrxCircuidView
//=============================================================================
//-----------------------------------------------------------------------------
const std::string FrxCircuidView::PROPERTY_ZORDER = "z_order";
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_Wires = 5.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_ProcessorNodes = 4.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_IO = 4.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_Knobs = 3.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_OnTop = 1.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_Default = FrxCircuidView::Z_OnTop;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_InteractiveStuff = Z_OnTop;
//-----------------------------------------------------------------------------
void FrxCircuidView::add(sdc::AComponentPtr comp, ZOrder zord, bool normalize) 
{
	if (normalize) {
		sd::Point2D loc = comp->getLocation();
		boost::geometry::add_point(loc, viewPort->getViewPosition());
		comp->setLocation(loc);
	}
	// order on insert:
	for (size_t i = 0; i<content->getComponentCount(); ++i) {
		AComponent::Ptr c = content->getComponent(i);
		ZOrder z = FLT_MAX;
		c->getClientProperty(PROPERTY_ZORDER, z);
		if (zord < z ) {
			comp->putClientProperty(PROPERTY_ZORDER, zord);
			content->add(comp, i);
			return;
		}
	}
	comp->putClientProperty(PROPERTY_ZORDER, zord);
	content->add(comp);
}
//-----------------------------------------------------------------------------
FrxCircuidView::FrxCircuidView() {
	setName("FrxCircuidView");
	//selection = FrxSelection::create();
	//add(selection, Z_InteractiveStuff); // !parent <= !!
}
//-----------------------------------------------------------------------------
void FrxCircuidView::remove(sdc::AComponentPtr comp) {
	FrxComponent::Ptr frxC = boost::shared_dynamic_cast<FrxComponent>(comp);
	if (frxC) { // fire removing event
		frxC->EventSender<OnRemoving>::notifyListeners(frxC.get(), 
			OnRemoving(getPtr()));
	}
	content->remove(comp);
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr 
FrxCircuidView::createComponentUI(sdcu::ALookAndFeelPtr laf) const
{
	return laf->getUI<FrxCircuidView>();
}
//-----------------------------------------------------------------------------
void FrxCircuidView::setStatusMessage(const std::string &txt, 
	const std::string &iconname) 
{
	statusMessage->setText(txt);
	if (iconname == "") {
		statusMessage->setIcon(sd::ISurface::Ptr());
		return;
	}
	sd::ISurface::Ptr icon = 
		sd::getResourceManager().getImage("StatusMessage.icon." + iconname);
	statusMessage->setIcon(icon);

	if (!statusMessage->getParent())
		return;
	statusMessage->getParent()->revalidate();
	statusMessage->redraw();
}
//-----------------------------------------------------------------------------
void FrxCircuidView::initStatusBar() {
	sdc::Panel::Ptr panel = sdc::Panel::create();
	panel->setLayout(sdc::FlowLayout::create(sdc::FlowLayout::LEFT, 0, 0));
	statusMessage = StatusLabel::create();
	sd::svg::graphicElements::Style style;
	sdc::ui::UIManager &m = sdc::ui::getUIManager();
	m.getProperty("StatusMessage.fontStyle", style);
	statusMessage->setFont( style.font());
	panel->add(statusMessage);
	Super::add(panel, sdc::BorderLayout::SOUTH, -1);
	setStatusMessage("Use the contextmenu0123456789", "hint");
}
//-----------------------------------------------------------------------------
void FrxCircuidView::postConstructor() {
	Super::setLayout(sdc::BorderLayout::create());
	// init mainview
	viewPort = sdc::Viewport::create();
	// init contentpane
	content = BgPane::create();
	content->setSize(sd::Dimension(FRX_MAX_VIEW, FRX_MAX_VIEW));
	viewPort->add(content);
	Super::add(viewPort);
	content->setLayout(sdc::ALayoutManagerPtr());
	// init selection
	selection = FrxSelection::create();
	add(selection, Z_InteractiveStuff);
	selection->setVisible(false);
	viewPort->setViewPosition(sd::Point2D(FRX_MAX_VIEW/2., FRX_MAX_VIEW/2.));
	initStatusBar();
}
//-----------------------------------------------------------------------------
namespace {
	typedef FrxCircuidView::ZOrder ZOrder;
	struct Filter {
		sd::Point2D loc;
		ZOrder start, end;
		Filter(const sd::Point2D &loc, ZOrder start, ZOrder end) :
		loc(loc), start(start), end(end) {}
		int operator()( sdc::AComponent::Ptr p ) {
			if (!p)
				return 0;
			if (loc==NULL_POINT2D) {
				return -1;
			}
			ZOrder z = FLT_MIN;
			p->getClientProperty(FrxCircuidView::PROPERTY_ZORDER, z);
			if (z < start)
				return 0; // continue searching
			if (z > end)
				return -1;  // stop searching
			sd::Point2D componentLoc = loc;
			boost::geometry::subtract_point(componentLoc, p->getLocation());
			if (p->contains(componentLoc)) {
				loc=NULL_POINT2D; // stop further searching
				return 1;
			}
			return 0;
		}
	};
} // namespace(s)
sdc::AComponentPtr FrxCircuidView::findComponentOnPoint(const sd::Point2D &p,
		FrxCircuidView::ZOrder _start, 
		FrxCircuidView::ZOrder _end
	)
{
	ZOrder start = ::std::min(_start, _end);
	ZOrder end = ::std::max(_start, _end);
	std::list<sdc::AComponentPtr> res;
	Filter filter(p, start, end);
	findComponents(res, filter);
	if (res.empty()){
		return sdc::AComponentPtr();
	}
	return res.back();
}
//-----------------------------------------------------------------------------
std::string FrxCircuidView::getStatusMessage() const {
	return statusMessage->getText();
}
//-----------------------------------------------------------------------------
void FrxCircuidView::hintMessage(const std::string &str) {
	if (usrMsg.length() > 0)
		return;
	setStatusMessage(str, "hint");
}
//-----------------------------------------------------------------------------
void FrxCircuidView::message(const std::string &str) {
	setStatusMessage(str, "default");
}
//-----------------------------------------------------------------------------
void FrxCircuidView::warnMessage(const std::string &str) {
	setStatusMessage(str, "warning");
}
//-----------------------------------------------------------------------------
void FrxCircuidView::errorMessage(const std::string &str) {
	::com::osMessageBox("Error:", str, ::com::MSG_ALERT);
}
//-----------------------------------------------------------------------------
void FrxCircuidView::setUserMessage(const std::string &txt, 
		const std::string &icon) 
{
	setStatusMessage(txt, icon);
}
}}} // namespace(s)
