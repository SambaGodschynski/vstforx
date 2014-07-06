/*
 * FrxSvgNodeUI.hpp
 *
 *  Created on: Mon Aug 20 10:46:25 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FrxSvgNodeUI_H
#define SAMBAG_FrxSvgNodeUI_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <gui/components/FrxComponent.hpp>
#include <gui/components/Forward.hpp>
#include "FrxSvgComponentUI.hpp"
#include <sambag/disco/components/Animation.hpp>
#include <sambag/disco/Tweens.hpp>
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
  * @class FrxSvgNodeUI.
  * Every Node has two areas. The Core which is represents the concrete
  * object and the Corona which is around the core. The corona can be used
  * for example to drag the object or for connecting.
  */
class FrxSvgNodeUI : public FrxSvgComponentUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxSvgComponentUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxSvgNodeUI> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxSvgNodeUI> WPtr;
protected:
    //-------------------------------------------------------------------------
    struct MouseListener;
    typedef boost::shared_ptr<MouseListener> MouseListenerPtr;
    MouseListenerPtr connect, move;
	//-------------------------------------------------------------------------
	virtual void installListeners(sdc::AComponent::Ptr c);
	//-------------------------------------------------------------------------
	virtual void installDefaults(sdc::AComponent::Ptr c);
	//-------------------------------------------------------------------------
	FrxSvgNodeUI(){}
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
    void startCoronaFadeIn();
    //-------------------------------------------------------------------------
    void startCoronaFadeOut();
	//-------------------------------------------------------------------------
	typedef boost::tuple<FrxNodePtr, FrxNodePtr, sd::Point2D> ConnectingComponents;
	//-------------------------------------------------------------------------
	ConnectingComponents getConnectingComponents(const sdc::events::MouseEvent &ev);
    //-------------------------------------------------------------------------
    virtual void setCoronaAlpha(double alpha);
public:
    //-------------------------------------------------------------------------
    sdc::SvgComponent::Dummy::Ptr getCorona() const;
    //-------------------------------------------------------------------------
    sdc::SvgComponent::Dummy::Ptr getCore() const;
    //-------------------------------------------------------------------------
    virtual sdc::SvgComponent::Dummy::Ptr getDragHandle() const {
        return getCorona();
    }
    //-------------------------------------------------------------------------
    virtual sdc::SvgComponent::Dummy::Ptr getConnectingHandle() const {
        return getCore();
    }
	//-------------------------------------------------------------------------
	virtual ~FrxSvgNodeUI();
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
	void onChildComponentMouse(void *src, const sdc::events::MouseEvent &ev);
    //-------------------------------------------------------------------------
    sdc::SvgComponent::Dummy::WPtr _corona;
private:
    //-------------------------------------------------------------------------
    double alphaEndValue;
	//-------------------------------------------------------------------------
	template <class T>
	struct CoronaAlpha : sdc::BasicUpdater<T> {
		typedef CoronaAlpha<T> ThisClass;
        FrxSvgNodeUI::WPtr _parent;
		T alpha;
		void update(const T& val){
            FrxSvgNodeUI::Ptr parent = _parent.lock();
            if (!parent) {
                return;
            }
            alpha = val;
            parent->setCoronaAlpha(val);
        }
		void finished(const T& val){
			update(val);
		}
		CoronaAlpha() : alpha( T() ) {}
	};
	typedef sdc::Animation<double, sdc::defaultTweens::DynamicTween, CoronaAlpha>
		FadeAnimation; 
	FadeAnimation::Ptr fadeAnimation;
public:
	//-------------------------------------------------------------------------
	double getCoronaAlpha() const {
		if (!fadeAnimation) {
			return 0.;
		}
		return fadeAnimation->alpha;
	}
	//-------------------------------------------------------------------------
	Ptr getPtr() const {
		return boost::dynamic_pointer_cast<FrxSvgNodeUI>(Super::getPtr());
	}
    //-------------------------------------------------------------------------
    virtual bool contains(sdc::AComponentPtr c, const sd::Point2D &p);
}; // FrxSvgNodeUI
}}}} // namespace(s)

#endif /* SAMBAG_FrxSvgNodeUI_H */
