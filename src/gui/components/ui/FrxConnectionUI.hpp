/*
 * FrxConnectionUI.hpp
 *
 *  Created on: Mon Aug 20 10:46:11 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCONNECTIONUI_H
#define SAMBAG_FRXCONNECTIONUI_H

#include <boost/shared_ptr.hpp>
#include <gui/components/FrxConcreteConnections.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <sambag/disco/IDiscoFactory.hpp>
#include "FrxComponentUI.hpp"
#include <gui/components/FrxNode.hpp>
#include <sambag/disco/Geometry.hpp>
#include <sambag/disco/components/events/MouseEvent.hpp>
#include <sambag/disco/components/Menu.hpp>
#include <sambag/disco/components/PopupMenu.hpp>
#include <sambag/math/VectorNCreator.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <gui/IFrxControl.hpp>
#include <sambag/disco/svg/graphicElements/Style.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxConnectionUIBase.
  */
class FrxConnectionUIBase : public FrxComponentUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxComponentUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxConnectionUIBase> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxConnectionUIBase() {}
private:
public:
	//-------------------------------------------------------------------------
	virtual ~FrxConnectionUIBase() {}
	//-------------------------------------------------------------------------
	/**
	 * adjust bounds considering line style.
	 */ 
	virtual void 
	adjustBoundingRect(sd::Rectangle &r, FrxConnection::Ptr c) const = 0;
}; // FrxConnectionUIBase
//=============================================================================
/** 
  * @class FrxConnectionUI.
  */
template <class _ConnectionType>
class FrxConnectionUI : public FrxConnectionUIBase {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxConnectionUIBase Super;
	//-------------------------------------------------------------------------
	typedef FrxConcreteConnection<_ConnectionType> _ConcreteConnection;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<_ConcreteConnection> ConcreteConnectionPtr;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxConnectionUI> Ptr;
	//-------------------------------------------------------------------------
	typedef FrxConnectionUI<_ConnectionType> Class;
protected:
	//-------------------------------------------------------------------------
	FrxConnectionUI() : _mouseEntered(false){}
	//-------------------------------------------------------------------------
	std::pair<sd::Point2D, sd::Point2D> 
	getConnectionPoints(ConcreteConnectionPtr c) const;
	//-------------------------------------------------------------------------
	void clipNode(ConcreteConnectionPtr c, 
		FrxNode::Ptr node, sd::IDrawContext::Ptr cn) const;
	//-------------------------------------------------------------------------
	void clipEndNodes(ConcreteConnectionPtr c, sd::IDrawContext::Ptr cn) const;
	//-------------------------------------------------------------------------
	virtual void installListeners(sdc::AComponent::Ptr c);
	//-------------------------------------------------------------------------
	virtual void installDefaults(sdc::AComponent::Ptr c);
	//-------------------------------------------------------------------------
	virtual void createPopupmenuEntries(sdc::PopupMenuPtr menu, 
		FrxCircuidViewPtr view, 
		FrxComponentPtr c);
private:
	//-------------------------------------------------------------------------
	bool _mouseEntered;
	//-------------------------------------------------------------------------
	sdsg::Style lineStyle;
	//-------------------------------------------------------------------------
	sdsg::Style lineHoverStyle;
	//-------------------------------------------------------------------------
	inline const sdsg::Style & determineStyle() const;
public:
	//-------------------------------------------------------------------------
	virtual void installUI(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	void mouseEntered(const sdce::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseExited(const sdce::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseClicked(const sdce::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void onMouse(void *src, const sdce::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void setStyleToContext(sd::IDrawContext::Ptr cn) const;
	//-------------------------------------------------------------------------
	virtual bool contains(sdc::AComponentPtr c, const sd::Point2D &p);
	//-------------------------------------------------------------------------
	/**
	 * adjust bounds considering line style.
	 */ 
	virtual void 
	adjustBoundingRect(sd::Rectangle &r, FrxConnection::Ptr c) const;
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res(new FrxConnectionUI());
		res->self = res;
		return res;
	}
	//-------------------------------------------------------------------------
	virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);
}; // FrxConnectionUI
///////////////////////////////////////////////////////////////////////////////
namespace {
//-----------------------------------------------------------------------------
template <class CT>
bool hasContextMenu() { return false; }
template <>
inline bool hasContextMenu<connectionTypes::IO>() { return true; }
template <>
inline bool hasContextMenu<connectionTypes::ProcessorInput>() { return false; }
template <>
inline bool hasContextMenu<connectionTypes::ProcessorOutput>() { return false; }
template <>
inline bool hasContextMenu<connectionTypes::ProcessorParameter>() { return false; }
template <>
inline bool hasContextMenu<connectionTypes::Parameter>() { return true; }
template <>
inline bool hasContextMenu<connectionTypes::ParameterOP>() { return false; }
//-----------------------------------------------------------------------------
template <class CT>
void getStyles(sdsg::Style &normal, sdsg::Style &hover) {}
//-----------------------------------------------------------------------------
template <>
inline void getStyles<connectionTypes::IO>(sdsg::Style &normal,
	sdsg::Style &hover)
{ 
	sdc::ui::getUIManager().getProperty("IOCn.style", normal);
	sdc::ui::getUIManager().getProperty("IOCn.hoverStyle", hover);
}
//-----------------------------------------------------------------------------
template <>
inline void getStyles<connectionTypes::ProcessorInput>(sdsg::Style &normal,
	sdsg::Style &hover)
{ 
	sdc::ui::getUIManager().getProperty("ProcessorInputCn.style", normal);
	sdc::ui::getUIManager().getProperty("ProcessorInputCn.hoverStyle", hover);
}
//-----------------------------------------------------------------------------
template <>
inline void getStyles<connectionTypes::ProcessorOutput>(sdsg::Style &normal,
	sdsg::Style &hover)
{ 
	sdc::ui::getUIManager().getProperty("ProcessorOutputCn.style", normal);
	sdc::ui::getUIManager().getProperty("ProcessorOutputCn.hoverStyle", hover);
}
//-----------------------------------------------------------------------------
template <>
inline void getStyles<connectionTypes::ProcessorParameter>(sdsg::Style &normal,
	sdsg::Style &hover)
{ 
	sdc::ui::getUIManager().getProperty("ProcessorParameterCn.style", normal);
	sdc::ui::getUIManager().getProperty("ProcessorParameterCn.hoverStyle", hover);
}
//-----------------------------------------------------------------------------
template <>
inline void getStyles<connectionTypes::Parameter>(sdsg::Style &normal,
	sdsg::Style &hover)
{ 
	sdc::ui::getUIManager().getProperty("ParameterCn.style", normal);
	sdc::ui::getUIManager().getProperty("ParameterCn.hoverStyle", hover);
}
//-----------------------------------------------------------------------------
template <>
inline void getStyles<connectionTypes::ParameterOP>(sdsg::Style &normal,
	sdsg::Style &hover)
{ 
	sdc::ui::getUIManager().getProperty("ParameterOPCn.style", normal);
	sdc::ui::getUIManager().getProperty("ParameterOPCn.hoverStyle", hover);
}
//-----------------------------------------------------------------------------
void _extendPopupMenu(sdc::PopupMenu::WPtr _menu,
    FrxCircuidViewWPtr _view, FrxComponentWPtr _c)
{
    sdc::PopupMenu::Ptr menu = _menu.lock();
    FrxCircuidViewPtr view = _view.lock();
    FrxComponentPtr c = _c.lock();
    if (!menu || !view || !c) {
        return;
    }
    
    sdc::Menu::Ptr smenu;
    
    c->getClientProperty("frxParameterConnection.ops.removemenu", smenu);

    if (!smenu) {
        smenu = sdc::Menu::create();
        smenu->setText("remove operator");
        menu->add(smenu);
        c->putClientProperty("frxParameterConnection.ops.removemenu", smenu);
    } else {
        //smenu->removeAll();
    }
    
    FrxConnection::Ptr connection =
        boost::dynamic_pointer_cast<FrxConnection>(c);
    if (!connection) {
        return;
    }
    
    IFrxControl &ctrl = getFrxControl(view);
    std::vector<IFrxControl::Operator> ops;
    ctrl.getOperators(view, connection, ops);
    if (ops.empty()) {
        return;
    }
    
    BOOST_FOREACH(const IFrxControl::Operator &op, ops) {
        sdc::MenuItem::Ptr item = sdc::MenuItem::create();
        item->setText(op.second);
        item->sce::EventSender<sdc::events::ActionEvent>::addTrackedEventListener (
            boost::bind(&IFrxControl::removeOperator, &ctrl, view, connection, op.first),
            c
        );
        smenu->add(item);
    }
}
//-----------------------------------------------------------------------------
template <class CT>
void _createPopupmenuEntries(sdc::PopupMenuPtr menu, 
	FrxCircuidViewPtr view, FrxComponentPtr c)
{
}
template <>
inline void _createPopupmenuEntries<connectionTypes::Parameter>(
	sdc::PopupMenuPtr menu, FrxCircuidViewPtr view, FrxComponentPtr c)
{
	// connection op's
	IFrxControl &ctrl = getFrxControl(view);
	// details
	sdc::MenuItem::Ptr item = sdc::MenuItem::create();
	item->setText("show details...");
	item->sce::EventSender<sdc::events::ActionEvent>::addTrackedEventListener (
		SAMBAG_CREATE_FRXCONTROL_CMD(ctrl, view, c, 
		&IFrxControl::showConnectionDetails),
		c
	);
	menu->add(item);
	// op's
	sdc::Menu::Ptr smenu = sdc::Menu::create();
	smenu->setText("add operator");
	menu->add(smenu);
    
    /*menu->sce::EventSender<sdc::PopupMenu::BeforeShowingEvent>::addTrackedEventListener(
        boost::bind(&_extendPopupMenu,
                    sdc::PopupMenu::WPtr(menu),
                    FrxCircuidViewWPtr(view),
                    FrxComponentWPtr(c)
        ), c
    );*/
    
	// get connection op's
	IFrxControl::ParameterCnOpTypeIds opIds;
	ctrl.getParameterCnOpTypeIds(view, opIds);
	BOOST_FOREACH(const IFrxControl::ParameterCnOpTypeId &id, opIds) {
		sdc::MenuItem::Ptr item = sdc::MenuItem::create();
		item->setText(sambag::com::toString(id));
		item->sce::EventSender<sdc::events::ActionEvent>::addTrackedEventListener (
			SAMBAG_CREATE_FRXCONTROL_CMD1(ctrl,view,c,&IFrxControl::addParamterCnOp, id),
			c
		);
		smenu->add(item);
	}
}

} // namespace
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class CT>
inline const sdsg::Style & FrxConnectionUI<CT>::determineStyle() const {
	if (_mouseEntered) {
		return lineHoverStyle;
	} 
	return lineStyle;
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::setStyleToContext(sd::IDrawContext::Ptr cn) const {
	const sdsg::Style & style = determineStyle(); 
	style.intoContext(cn);
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::installListeners(sdc::AComponent::Ptr c) {
	if ( !hasContextMenu<CT>() )
		return;
	c->sce::EventSender<sdc::events::MouseEvent>::addTrackedEventListener(
		boost::bind(&Class::onMouse, this, _1, _2),
		getPtr()
	);
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::installDefaults(sdc::AComponent::Ptr c) {
	Super::installDefaults(c);
	getStyles<CT>(lineStyle, lineHoverStyle);
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::installUI(sdc::AComponentPtr c) {
	Super::installUI(c);
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::adjustBoundingRect(sd::Rectangle &r, 
	FrxConnection::Ptr c) const 
{
	sambag::com::Number lw = determineStyle().strokeWidth();
	r.x( r.x() - lw );
	r.y( r.y() - lw );
	r.width( r.width() + lw*2. );
	r.height( r.height() + lw*2. );
}
//-----------------------------------------------------------------------------
template <class CT>
std::pair<sd::Point2D, sd::Point2D> 
FrxConnectionUI<CT>::getConnectionPoints(ConcreteConnectionPtr ccn) const {
	sd::Point2D aLoc = ccn->getSrcComponent()->getLocation();
	boost::geometry::add_point(aLoc, ccn->getSrcComponent()->getPivot());
	sd::Point2D bLoc = ccn->getDstComponent()->getLocation();
	boost::geometry::add_point(bLoc, ccn->getDstComponent()->getPivot());
	return std::make_pair(aLoc, bLoc);
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::clipNode(ConcreteConnectionPtr c, FrxNode::Ptr node, 
	sd::IDrawContext::Ptr cn) const 
{
	sd::Point2D loc = node->getLocation();
	boost::geometry::add_point(loc, node->getPivot());
	boost::geometry::subtract_point(loc, c->getLocation());
	cn->arc(loc, node->getRadius());
    cn->clip();
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::clipEndNodes(ConcreteConnectionPtr c, 
	sd::IDrawContext::Ptr cn) const
{
	FrxNode::Ptr a = boost::dynamic_pointer_cast<FrxNode>( c->getSrcComponent() );
	FrxNode::Ptr b = boost::dynamic_pointer_cast<FrxNode>( c->getDstComponent() );
	if (!a && !b) {
		return;
	}
	cn->setFillRule(sd::IDrawContext::FILL_RULE_EVEN_ODD);
	if (a) {
		cn->rect(sd::Rectangle(0, 0, c->getWidth(), c->getHeight()));
		clipNode(c, a, cn);
	}
	if (b) {
		cn->rect(sd::Rectangle(0, 0, c->getWidth(), c->getHeight()));
		clipNode(c, b, cn);
	}
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	//Super::draw(cn, c);
    typename _ConcreteConnection::Ptr ccn =
		boost::dynamic_pointer_cast<_ConcreteConnection>(c);
	SAMBAG_ASSERT(ccn);
	
	std::pair<sd::Point2D, sd::Point2D> points = getConnectionPoints(ccn);
    if (points.first==points.second) {
        // no line here
        return;
    }
	boost::geometry::subtract_point(points.first, ccn->getLocation());
	boost::geometry::subtract_point(points.second, ccn->getLocation());

	clipEndNodes(ccn, cn);
	cn->moveTo(points.first);
	cn->lineTo(points.second);
	setStyleToContext(cn);
    cn->stroke();
}
//-----------------------------------------------------------------------------
SAMBAG_PROPERTY_TAG(FrxConnectionHitDistance, "FrxConnection.hitDistance");
//-----------------------------------------------------------------------------
template <class CT>
bool FrxConnectionUI<CT>::contains(sdc::AComponentPtr c, const sd::Point2D &p) {
	/*
	Q = hitPoint, P1=linienAnfang, P2=linienEnde
	1. Richtungsvektor ausrechnen
		U.x = P2.x - P1.x
		U.y = P2.y - P1.y
	2. Dann den Richtungsvektor normieren
		Len = abs (u)
		if Len < 0.0001 Fehler!
		U.x = U.x / Len
		U.y = U.y / Len
	3. Orthogonal Projizieren
		Lambda = (Q.x - P1.x) * U.x + (Q.y - P1.y) * U.y
	4. Fusspunkt ausrechnen
		 q - ( p1 + (lambda * u) );
	5.	Auswerten
	    len = abs(d);
		getroffen wenn len<toleraz
*/	
	using namespace sambag::com;
	sd::Point2D _q = p;
	const sd::Rectangle &bounds = c->getBounds();
	boost::geometry::add_point(_q, bounds.x0());
	if (!bounds.contains(_q)) {
		return false;
	}
	// prepare vector data
	sd::Coordinate distance = 
		sdc::ui::getUIPropertyCached<FrxConnectionHitDistance>(sd::Coordinate(10.));
	typename _ConcreteConnection::Ptr ccn = 
		boost::dynamic_pointer_cast<_ConcreteConnection>(c);
	typedef sambag::math::VectorN<Number, 2> Vector2D;
	std::pair<sd::Point2D, sd::Point2D> points = getConnectionPoints(ccn);
	Vector2D p1 = sambag::math::createVector((Number)points.first.x(), 
		(Number)points.first.y());
	Vector2D p2 = sambag::math::createVector((Number)points.second.x(), 
		(Number)points.second.y());
	Vector2D q = sambag::math::createVector((Number)_q.x(), 
		(Number)_q.y());
	// 1.
	Vector2D u = p2 - p1;
	// 2.
	sd::Coordinate len = u.abs();
	if (len<0.0001)
		return false;
	u/=len;
	// 3.
	Number lambda = (q.get<1>() - p1.get<1>()) * u.get<1>() + 
		(q.get<2>() - p1.get<2>()) * u.get<2>();
	// 4.
	Vector2D d = q - ( p1 + (u * lambda));
	// 5.
	Number dist = d.abs();
	return dist < distance;
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::mouseEntered(const sdce::MouseEvent &ev) {
	_mouseEntered = true;
	sdc::AComponent::Ptr c = ev.getSource()->getParent();
	if (c) {
		c->redraw();
	}
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::mouseClicked(const sdce::MouseEvent &ev) {
	sdc::AComponent::Ptr c = ev.getSource();
	FrxCircuidView::Ptr circ = c->getFirstContainer<FrxCircuidView>();
	SAMBAG_ASSERT(circ);
	getFrxControl(circ).handleContextMenuPopup(ev);
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::mouseExited(const sdce::MouseEvent &ev) {
	_mouseEntered = false;
	sdc::AComponent::Ptr c = ev.getSource()->getParent();
	if (c) {
		c->redraw();
	}
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::onMouse(void *src, const sdce::MouseEvent &ev) {
	enum { Filter = sdce::MouseEvent::DISCO_MOUSE_ENTERED |
		sdce::MouseEvent::DISCO_MOUSE_EXITED |
		sdce::MouseEvent::DISCO_MOUSE_CLICKED
	};
	sdce::MouseEventSwitch<Filter>::delegate(ev, *this);
}
//-----------------------------------------------------------------------------
template <class CT>
void FrxConnectionUI<CT>::createPopupmenuEntries(sdc::PopupMenuPtr menu,
    FrxCircuidViewPtr view, FrxComponentPtr c)
{
	Super::createPopupmenuEntries(menu, view, c);
	_createPopupmenuEntries<CT>(menu, view, c);
}
}}}} // namespace(s)

#endif /* SAMBAG_FRXCONNECTIONUI_H */
