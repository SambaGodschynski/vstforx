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
#include <sambag/disco/components/ViewPort.hpp>
#include <sambag/math/Matrix.hpp>
#include <sambag/disco/svg/graphicElements/Style.hpp>
#include <sambag/disco/components/Window.hpp>
#include <sambag/disco/components/SolidBorder.hpp>
#include <sambag/disco/svg/StyleParser.hpp>
#include <sambag/math/Matrix.hpp>
#include <com/Settings.h>
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_primitives.hpp>

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
	sd::IPattern::Ptr shaderPat;
	sd::ISurface::Ptr logo;
    sd::ColorRGBA bg;
    double parallaxEffect;
    sd::Matrix bgTrans;
    sdc::Viewport::WPtr parent;
	virtual void postConstructor();
	void drawShadingLayer(sd::IDrawContext::Ptr cn, 
		const sd::Rectangle &r);
public:
	//-------------------------------------------------------------------------
	/**
	 * @override
	 * do not find any objects which have set the client property
	 * FrxCircuidView.inactive @see putClientProperty
	 */
	virtual sdc::AComponentPtr findComponentAt(const sd::Point2D &p,
		bool includeSelf);
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(BgPane)
	virtual void drawComponent(sd::IDrawContext::Ptr cn);
};
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
namespace {
	sd::IPattern::Ptr _createSPattern() {
		sd::IGradient::ColorStops stops;
		sdc::ui::UIManager &ui = sdc::ui::getUIManager();
		ui.getProperty("FrxCircuidView.bg.gradient.colorStops", stops);
		sd::ILinearPattern::Ptr sol = 
			sd::getDiscoFactory()->createLinearPattern(sd::Point2D(0,0), sd::Point2D(0,600));
		sol->addColorStops(stops);
		return sol;
	}
	void _getViewportRect(sdc::AComponentPtr view, sd::Rectangle &res) {
		sdc::Viewport::Ptr v =
			boost::dynamic_pointer_cast<sdc::Viewport>(view->getParent());
		if (!v) {
			return;
		}
		sd::Point2D p = v->getViewPosition();
		res = v->getBounds();
		res.x( p.x() );
		res.y( p.y() );
	}
}
void BgPane::drawShadingLayer(sd::IDrawContext::Ptr cn, 
	const sd::Rectangle &r) 
{
	if (!shaderPat) {
		shaderPat = _createSPattern();
	}
	cn->rect(r);
	shaderPat->setMatrix(
		sambag::math::translate2D(
			-r.x(),
			-r.y()
		)
	);
	cn->setFillPattern(shaderPat);
	cn->fill();
}
void drawNotifictaion(sd::IDrawContext::Ptr cn, const sd::Rectangle &r, const std::string &txt)
{
	cn->setFont(cn->getCurrentFont().setSize(36.));
	sd::Rectangle tx = cn->textExtends(txt);
	sambag::com::Number x = r.x();
	sambag::com::Number y = r.y() + tx.height();
	cn->moveTo(sd::Point2D(x,y));
	cn->textPath(txt);
	cn->setFillColor(sd::ColorRGBA(0.6, 0.6, 0.6, 1));
	cn->fill();
}

void drawDemoNotifictaion(sd::IDrawContext::Ptr cn, const sd::Rectangle &r)
{
	if (!globFrxIsDemo()) {
		return;
	}
    drawNotifictaion(cn, r, "DEMO VERSION");
}
void drawLogo(sd::IDrawContext::Ptr cn, sd::ISurface::Ptr logo, const sd::Rectangle &r)
{
    if (!logo) {
        return;
    }
    sd::Rectangle bounds = logo->getSize();
    bounds.x( r.x() + r.width()/2. - bounds.width()/2. );
    bounds.y( r.y() + r.height()/2. - bounds.height()/2. );
    cn->save();
    cn->translate(bounds.x0());
	cn->drawSurface(logo);
    cn->restore();
}
void BgPane::drawComponent(sd::IDrawContext::Ptr cn) {
	if (!pat) {
		Super::drawComponent(cn);
		return;
	}
	sd::Rectangle r;
	_getViewportRect(getPtr(), r);
    
    
    if (parallaxEffect>0.) {
        sdc::Viewport::Ptr vp = parent.lock();
        if (!vp) {
            parent = vp = getFirstContainer<sdc::Viewport>();
            if (!vp) {
                SAMBAG_LOG_ERR<<"BgPane::drawComponent() Viewport==NULL";
                return;
            }
        }
        sd::Point2D p = vp->getViewPosition();
        pat->setMatrix(
            boost::numeric::ublas::prod(bgTrans,
            sd::translate2D(-p.x()*parallaxEffect, -p.y()*parallaxEffect))
        );
    }
    
	cn->setFillPattern(pat);
	cn->rect(sd::Rectangle(0, 0, getWidth(), getHeight()));
	cn->fill();
	drawDemoNotifictaion(cn, r);
    drawLogo(cn, logo, r);
	drawShadingLayer(cn, r);
}
//-----------------------------------------------------------------------------
void BgPane::postConstructor() {
	sdc::ui::UIManager &ui = sdc::ui::getUIManager();
	sd::ISurface::Ptr fillImg = 
		sd::getResourceManager().getImage("FrxCircuidView.image");
	ui.getProperty("FrxCircuidView.bgColor", bg);
	if (!fillImg)
		return;
	pat = sd::getDiscoFactory()->createSurfacePattern(fillImg);
	if (!pat)
		return;
	bgTrans = IDENTITY_MATRIX;
	ui.getProperty("FrxCircuidView.bgTransfomation", bgTrans);
	sd::IPattern::Extend e = sd::IPattern::DISCO_EXTEND_REPEAT;
	ui.getProperty("FrxCircuidView.bgExtend", e);
	double opac = 0.3;
	ui.getProperty("FrxCircuidView.bgOpacity", opac);
	pat->setMatrix(bgTrans);
	pat->setExtendType(e);
	pat->setOpacity(opac);
    logo = sd::getResourceManager().getImage("FrxCircuidView.logo");
    parallaxEffect = 0.;
    ui.getProperty("FrxCircuidView.bg.parallaxEffect", parallaxEffect);
}
//-----------------------------------------------------------------------------
sdc::AComponentPtr BgPane::findComponentAt(const sd::Point2D &p,
		bool includeSelf)
{
	using namespace boost;
	SAMBAG_BEGIN_SYNCHRONIZED(getTreeLock())
		BOOST_FOREACH(sdc::AComponent::Ptr comp, getComponents()) {
			sd::Point2D trP = p;
			geometry::subtract_point(trP, comp->getLocation());
			if (comp && 
				comp->isVisible() &&
				comp->contains(trP))
			{
				bool inactive = false;
				comp->getClientProperty("FrxCircuidView.inactive", inactive);
				if (inactive) {
					continue;
				}
				sdc::AContainer::Ptr con = 
					boost::dynamic_pointer_cast<sdc::AContainer>(comp);
				if (con) {
					sdc::AComponent::Ptr deeper = con->findComponentAt(
						trP,
						includeSelf);
					if (deeper) {
						return deeper;
					}
				} else {
					return comp;
				}
			}
		}
	SAMBAG_END_SYNCHRONIZED
	if (contains(p) && includeSelf) {
		return getPtr();
	}
	return sdc::AComponent::Ptr();
}
}// namespace(s)
//=============================================================================
//  Class FrxCircuidView
//=============================================================================
//-----------------------------------------------------------------------------
const std::string FrxCircuidView::PROPERTY_ZORDER = "z_order";
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_Flags = 1.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_Wires = 5.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_ProcessorNodes = 4.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_IO = 4.5f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_Knobs = 3.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_OnTop = 1.f;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_Default = FrxCircuidView::Z_OnTop;
//-----------------------------------------------------------------------------
const float FrxCircuidView::Z_InteractiveStuff = Z_OnTop;
//-----------------------------------------------------------------------------
const float FrxCircuidView::ZArea_BeginNodes = Z_Knobs;
//-----------------------------------------------------------------------------
const float FrxCircuidView::ZArea_EndNodes = Z_IO;
//-----------------------------------------------------------------------------
std::string FrxCircuidView::uniqueName(const std::string &x) {
    bool serializing = false;
    getClientProperty("serializing", serializing);
    if (serializing) {
        return x;
    }
    using namespace boost::xpressive;
	mark_tag tName(1), tCounter(2);
	cregex pat = (tName= -+_) >> "_" >> (tCounter= +_d) >> eol;
	cmatch what;
    std::string name;
    int counter;
	if(regex_search(x.c_str(), what, pat)) {
		std::stringstream ss;
		ss << what[tCounter];
		ss >> counter;
		name = what[tName];
	}
    if (name.length() == 0) {
        name = x;
    }
    NameMap::iterator it = nameMap.find(name);
    if (it==nameMap.end()) {
        nameMap[name] = 0;
        return name;
    }
    ++(it->second);
    return name + "_" + sambag::com::toString(it->second);
    
}
//-----------------------------------------------------------------------------
void FrxCircuidView::add(sdc::AComponentPtr comp, ZOrder zord, bool normalize) 
{
    comp->setName( uniqueName(comp->getName()) );
	SAMBAG_BEGIN_SYNCHRONIZED(getTreeLock())
	if (normalize) {
		sd::Point2D loc = comp->getLocation();
		boost::geometry::add_point(loc, viewPort->getViewPosition());
		comp->setLocation(loc);
	}
	bool inserted = false;
	// order on insert:
	for (size_t i = 0; i<content->getComponentCount(); ++i) {
		AComponent::Ptr c = content->getComponent(i);
		ZOrder z = FLT_MAX;
		c->getClientProperty(PROPERTY_ZORDER, z);
		if (zord < z ) {
			comp->putClientProperty(PROPERTY_ZORDER, zord);
			content->add(comp, i);
			inserted = true;
			break;
		}
	}
	if (!inserted) {
		comp->putClientProperty(PROPERTY_ZORDER, zord);
		content->add(comp);
	}
	FrxComponent::Ptr frxC =
		boost::dynamic_pointer_cast<FrxComponent>(comp);
	if (!frxC)
		return;

	sce::EventSender<FrxCircuidViewEvent>::notifyListeners( this,
		FrxCircuidViewEvent(FrxCircuidViewEvent::ComponentAdded, frxC)
	);
	SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
FrxCircuidView::FrxCircuidView() {
	setName("FrxCircuidView");
	//selection = FrxSelection::create();
	//add(selection, Z_InteractiveStuff); // !parent <= !!
}
//-----------------------------------------------------------------------------
FrxCircuidView::~FrxCircuidView() {
    
}
//-----------------------------------------------------------------------------
void FrxCircuidView::open() {
	fireViewEvent(FrxCircuidViewEvent::OnOpening);
}
//-----------------------------------------------------------------------------
void FrxCircuidView::close() {
	fireViewEvent(FrxCircuidViewEvent::OnClosing);
}
//-----------------------------------------------------------------------------
void FrxCircuidView::fireViewEvent(FrxCircuidViewEvent::Type type, FrxComponentPtr c)
{
	sce::EventSender<FrxCircuidViewEvent>::notifyListeners( this,
		FrxCircuidViewEvent(type, c)
	);
}
//-----------------------------------------------------------------------------
void FrxCircuidView::remove(sdc::AComponentPtr comp) {
	SAMBAG_BEGIN_SYNCHRONIZED(getTreeLock())
	FrxComponent::Ptr frxC = boost::dynamic_pointer_cast<FrxComponent>(comp);
	if (frxC) { // fire removing event
		frxC->sce::EventSender<OnRemoving>::notifyListeners(frxC.get(),
			OnRemoving(getPtr()));
	}
	content->remove(comp);
	if (frxC) {
		fireViewEvent(FrxCircuidViewEvent::ComponentRemoved, frxC);
	}
	SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
sdcu::AComponentUIPtr 
FrxCircuidView::createComponentUI(sdcu::ALookAndFeelPtr laf) const
{
	return laf->getUI<FrxCircuidView>();
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
	statusBar = FrxStatusBar::create();
	Super::add(statusBar, sdc::BorderLayout::SOUTH, -1);
}
//-----------------------------------------------------------------------------
int FrxCircuidView::getIndexOf(ZOrder order) const {
	sdc::AContainer::Ptr cnt = getContentPane(); 
	int startIndex = 0;
	int endIndex = cnt->getComponentCount();
	// TODO: impl. O(log(n))
	for (int i=0; i<endIndex; ++i) {
		sdc::AComponentPtr c = cnt->getComponent(i);
		ZOrder z = FLT_MIN;
		c->getClientProperty(FrxCircuidView::PROPERTY_ZORDER, z);
		if (z>=order) {
			return i;
		}
	}
	return -1;
}
//-----------------------------------------------------------------------------
std::string FrxCircuidView::componentsToString() const {
	std::stringstream ss;
	sdc::AContainer::Ptr cnt = getContentPane(); 
	int endIndex = cnt->getComponentCount();
	ss<<"{";
	for (int i=0; i<endIndex; ++i) {
		sdc::AComponentPtr c = cnt->getComponent(i);
		ZOrder z = FLT_MIN;
		c->getClientProperty(FrxCircuidView::PROPERTY_ZORDER, z);
		ss<<c->getName()<<"["<<z<<"]";
		if (i<endIndex-1) {
			ss<<", ";
		}
	}
	ss<<"}";
	return ss.str();
}
//-----------------------------------------------------------------------------
namespace {
	typedef FrxCircuidView::ZOrder ZOrder;
	struct Filter {
		sd::Point2D loc;
		ZOrder end;
		Filter(const sd::Point2D &loc, ZOrder end) :
		loc(loc), end(end) {}
		int operator()( sdc::AComponent::Ptr p ) {
			if (!p)
				return 0;
			if (!p->isVisible()) {
				return 0;
			}
			if (loc==NULL_POINT2D) {
				return -1;
			}
			ZOrder z = FLT_MIN;
			p->getClientProperty(FrxCircuidView::PROPERTY_ZORDER, z);
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
	Filter filter(p, end);
	int startIndex = getIndexOf(start);
	findComponents(res, filter, startIndex); // set the endindex isn't really
	                                         // useful because we have to iterate
	                                         // through the elements anyway
	if (res.empty()){
		return sdc::AComponentPtr();
	}
	return res.back();
}
//-----------------------------------------------------------------------------
std::string FrxCircuidView::getStatusMessage() const {
	return statusBar->getStatusLabel()->getText();
}
//-----------------------------------------------------------------------------
void FrxCircuidView::hintMessage(const std::string &str) {
	if (usrMsg.length() > 0)
		return;
	statusBar->setStatusMessage(str, "hint");
}
//-----------------------------------------------------------------------------
void FrxCircuidView::message(const std::string &str) {
	statusBar->setStatusMessage(str, "default");
}
//-----------------------------------------------------------------------------
void FrxCircuidView::warnMessage(const std::string &str) {
	statusBar->setStatusMessage(str, "warning");
}
//-----------------------------------------------------------------------------
void FrxCircuidView::errorMessage(const std::string &str) {
	::com::osMessageBox("Error:", str, ::com::MSG_ALERT);
}
//-----------------------------------------------------------------------------
void FrxCircuidView::setUserMessage(const std::string &txt, 
		const std::string &icon) 
{
	statusBar->setStatusMessage(txt, icon);
}
//-----------------------------------------------------------------------------
void FrxCircuidView::requestEditorResize(const sd::Dimension &size) {
	if (!rszHandler) {
		return;
	}
	rszHandler((int)size.width(), (int)size.height());
	using namespace sambag::disco::components;
	Window::Ptr win = getFirstContainer<Window>();
	if (!win) {
		return;
	}
	win->setWindowSize(size);
}
//-----------------------------------------------------------------------------
void FrxCircuidView::setEditorResizeHandler(const EditorResizeHandler &f) {
	rszHandler = f;
}
}}} // namespace(s)



