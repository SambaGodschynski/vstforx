/*
 * FrxNodeUI.hpp
 *
 *  Created on: Mon Aug 20 10:46:25 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXNODEUI_H
#define SAMBAG_FRXNODEUI_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <gui/components/FrxComponent.hpp>
#include <gui/components/Forward.hpp>
#include "FrxComponentUI.hpp"
#include <sambag/disco/components/Timer.hpp>
#include <sambag/disco/components/events/MouseEvent.hpp>
#include <sambag/disco/svg/graphicElements/Line.hpp>
#include <sambag/disco/components/ComponentWrapper.hpp>
#include <sambag/com/ArithmeticWrapper.hpp>
#include <boost/tuple/tuple.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxNodeUI.
  * Every Node has two areas. The Core which is represents the concrete
  * object and the Corona which is around the core. The corona can be used
  * for example to drag the object or for connecting.
  */
class FrxNodeUI : public FrxComponentUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxComponentUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxNodeUI> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxNodeUI> WPtr;
protected:
	//-------------------------------------------------------------------------
	virtual void installListeners(sdc::AComponent::Ptr c);
	//-------------------------------------------------------------------------
	virtual void installDefaults(sdc::AComponent::Ptr c);
	//-------------------------------------------------------------------------
	virtual void drawCorona(sd::IDrawContext::Ptr cn, sdc::AComponent::Ptr c);
	//-------------------------------------------------------------------------
	enum MouseContext {NONE, DRAG, CONNECT};
	//-------------------------------------------------------------------------
	sambag::com::ArithmeticWrapper<int> context;
	//-------------------------------------------------------------------------
	bool hitsCorona(sdc::AComponentPtr c, const sd::Point2D &p) const;
	//-------------------------------------------------------------------------
	bool hitsCore(sdc::AComponentPtr c, const sd::Point2D &p) const;
	//-------------------------------------------------------------------------
	FrxNodeUI(){}
	//-------------------------------------------------------------------------
	virtual void installUI(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	// mouse events
	sd::Point2D clickLoc;
	//-------------------------------------------------------------------------
	typedef sdc::ComponentWrapper<sdsg::Line> Line;
	//-------------------------------------------------------------------------
	Line::Ptr toConnect;
	//-------------------------------------------------------------------------
	virtual int 
		determineContext(const sdc::events::MouseEvent &ev) const;
	//-------------------------------------------------------------------------
	// MouseActions on object:
	virtual void drag(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void beginConnecting(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void connecting(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void endConnecting(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void use(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	sambag::com::Number coronaAlpha;
	//-------------------------------------------------------------------------
	typedef boost::tuple<FrxNodePtr, FrxNodePtr, sd::Point2D> ConnectingComponents;
	//-------------------------------------------------------------------------
	ConnectingComponents getConnectingComponents(const sdc::events::MouseEvent &ev);
public:
	//-------------------------------------------------------------------------
	// MouseEvents
	void mousePressed(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseReleased(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseEntered(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseExited(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseClicked(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseMoved(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseDragged(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void mouseWheelMoved(const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	void onMouse(void *src, const sdc::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void createPopupmenuEntries(sdc::PopupMenuPtr menu, 
		FrxCircuidViewPtr view, 
		FrxComponentPtr c)
	{
	}
private:
	//-------------------------------------------------------------------------
	sambag::com::ArithmeticWrapper<bool> inside;
	//-------------------------------------------------------------------------
	bool fadeIn;
	//-------------------------------------------------------------------------
	sdc::Timer::Ptr fadeTimer;
public:
	//-------------------------------------------------------------------------
	/**
	 * Returns true if the specified x,y location
	 * is contained within the look and feel's defined
	 * shape of the specified component.
	 * @param c
	 * @param x
	 * @param y
	 * @return
	 */
	virtual bool contains(sdc::AComponentPtr c, const sd::Point2D &p);
	//-------------------------------------------------------------------------
	Ptr getPtr() const {
		return boost::shared_dynamic_cast<FrxNodeUI>(Super::getPtr());
	}
	//-------------------------------------------------------------------------
	virtual bool usesCorona() const { return true; }
	//-------------------------------------------------------------------------
	virtual sambag::com::Number getCoreRadius(sdc::AComponentPtr c) const {
		return 25.;
	}
	//-------------------------------------------------------------------------
	virtual sambag::com::Number getCoronaRadius(sdc::AComponentPtr c) const {
		return getCoreRadius(c) + 10.;
	}
	//-------------------------------------------------------------------------
	/**
	 * Paints the specified component appropriately for the look and feel.
	 * @param cn
	 * @param c
	 */
	virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);

}; // FrxNodeUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXNODEUI_H */
