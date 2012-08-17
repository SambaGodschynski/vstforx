/*
 * FrxComponentUI.hpp
 *
 *  Created on: Tue Aug 17 17:33:20 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCOMPONENT_H
#define SAMBAG_FRXCOMPONENT_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/ui/IButtonUI.hpp>
#include <sambag/disco/components/AButton.hpp>
#include "BasicButtonListener.hpp"
#include <sambag/disco/svg/HtmlColors.hpp>
#include <sambag/disco/IDiscoFactory.hpp>
#include <sambag/disco/ISurface.hpp>
#include <sambag/disco/IPattern.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 

//=============================================================================
/** 
  * @class FrxComponentUI.
  */
template <class ButtonModell>
class FrxComponentUI  {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef IButtonUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxComponentUI<ButtonModell> > Ptr;
	//-------------------------------------------------------------------------
	typedef AButton<ButtonModell> AbstractButton;
protected:
	//-------------------------------------------------------------------------
	FrxComponentUI();
private:
	//-------------------------------------------------------------------------
	IPattern::Ptr bk, roll, press;
public:
	//-------------------------------------------------------------------------
	virtual bool contains(AComponent::Ptr c, const Point2D &p);
	//-------------------------------------------------------------------------
	void onButtonStateChanged(void *src, const
			typename AbstractButton::StateChangedEvent &ev);
	//-------------------------------------------------------------------------
	/**
	 * Configures the specified component appropriately for the look and feel.
	 * @param c
	 */
	virtual void installUI(AComponentPtr c);
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new FrxComponentUI<ButtonModell>());
	}
	//-------------------------------------------------------------------------
	/**
	 * Paints the specified component appropriately for the look and feel.
	 * @param cn
	 * @param c
	 */
	virtual void draw(IDrawContext::Ptr cn, AComponentPtr c);
	//-------------------------------------------------------------------------
	/**
	 * Returns the specified component's maximum size appropriate for the
	 * look and feel.
	 * @param c
	 * @return
	 */
	virtual Dimension getMaximumSize(AComponentPtr c);
	//-------------------------------------------------------------------------
	virtual Dimension getMinimumSize(AComponentPtr c);
	//-------------------------------------------------------------------------
	/**
	 * Returns the specified component's preferred size appropriate for the
	 * look and feel.
	 * @param c
	 * @return
	 */
	virtual Dimension getPreferredSize(AComponentPtr c);
}; // FrxComponentUI
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class ButtonModell>
FrxComponentUI<ButtonModell>::FrxComponentUI() {
	ILinearPattern::Ptr lp =
		getDiscoFactory()->createLinearPattern(Point2D(), Point2D(0.,10.));
	lp->addColorStop(svg::HtmlColors::getColor("white"), 0);
	lp->addColorStop(svg::HtmlColors::getColor("white"), 0.5);
	lp->addColorStop(svg::HtmlColors::getColor("lavender"), .51);
	lp->addColorStop(svg::HtmlColors::getColor("lavender"), 1.0);
	bk = lp;
	lp =
		getDiscoFactory()->createLinearPattern(Point2D(), Point2D(0.,10.));
	lp->addColorStop(svg::HtmlColors::getColor("white"), 0);
	lp->addColorStop(svg::HtmlColors::getColor("slategrey"), 1.0);
	press = lp;
	lp =
		getDiscoFactory()->createLinearPattern(Point2D(), Point2D(0.,10.));
	lp->addColorStop(svg::HtmlColors::getColor("white"), 0);
	lp->addColorStop(svg::HtmlColors::getColor("lightblue"), 1.0);
	roll = lp;
}
//-----------------------------------------------------------------------------
template <class ButtonModell>
void FrxComponentUI<ButtonModell>::draw(IDrawContext::Ptr cn, AComponentPtr c) {
	typename AbstractButton::Ptr b = boost::shared_dynamic_cast<AbstractButton>(c);
	if (!b)
		return;
	// consider clipping
	cn->translate(Point2D(2,2));
	cn->scale(Point2D(0.8, 0.8));

	if (b->isButtonRollover()) {
		if (b->isButtonPressed())
			//cn->setFillColor(svg::HtmlColors::getColor("slategrey"));
			cn->setFillPattern(press);
		else
			cn->setFillPattern(roll);
			//cn->setFillColor(svg::HtmlColors::getColor("lightblue"));
	} else
		//cn->setFillColor(svg::HtmlColors::getColor("aliceblue"));
		cn->setFillPattern(bk);
	cn->setStrokeColor(c->getForeground());
	cn->rect(Rectangle(0,0,c->getWidth(), c->getHeight()), 5);
	cn->fill();
	cn->rect(Rectangle(0,0,c->getWidth(), c->getHeight()), 5);
	cn->setStrokeWidth(1);
	cn->stroke();
	cn->setFont(b->getFont());
	std::string str = b->getText();
	Rectangle txt = cn->textExtends(str);
	cn->setFillColor(c->getForeground());
	cn->translate( Point2D( 10,
			c->getHeight() / 2.0 + txt.getHeight() / 2.0
	));
	cn->textPath(str);
	cn->fill();
}
//-----------------------------------------------------------------------------
template <class ButtonModell>
void FrxComponentUI<ButtonModell>::installUI(AComponentPtr c) {
	typename AbstractButton::Ptr b =
			boost::shared_dynamic_cast<AbstractButton>(c);
	BOOST_ASSERT(b);
	typedef BasicButtonListener<ButtonModell> ButtonListener;
	b->EventSender<events::MouseEvent>::addTrackedEventListener(
			boost::bind(&ButtonListener::onMouseEvent, _1, _2),
			b
	);
	b->EventSender<typename ButtonModell::StateChangedEvent>::
	addTrackedEventListener(
			boost::bind(&FrxComponentUI<ButtonModell>::onButtonStateChanged,
					this, _1, _2),
			b
	);
}
//-----------------------------------------------------------------------------
template <class ButtonModell>
void FrxComponentUI<ButtonModell>::onButtonStateChanged(void *src, const
			typename AbstractButton::StateChangedEvent &ev)
{
	const AbstractButton *_btn =
			dynamic_cast<const AbstractButton*>(&(ev.getSrc()));
	BOOST_ASSERT(_btn);
	typename AbstractButton::Ptr btn = _btn->getPtr();
	btn->redraw();
}
//------------------------------------------------------------------------------
template <class ButtonModell>
Dimension FrxComponentUI<ButtonModell>::getMaximumSize(AComponentPtr c) {
	return getPreferredSize(c);
}
//-----------------------------------------------------------------------------
template <class ButtonModell>
Dimension FrxComponentUI<ButtonModell>::getMinimumSize(AComponentPtr c) {
	return getPreferredSize(c);
}
//-----------------------------------------------------------------------------
template <class ButtonModell>
Dimension FrxComponentUI<ButtonModell>::getPreferredSize(AComponentPtr c) {
	typename AbstractButton::Ptr b =
			boost::shared_dynamic_cast<AbstractButton>(c);
	IDrawContext::Ptr cn = getDiscoFactory()->createContext();
	// TODO: handle font style/size
	cn->setFont(b->getFont());
	Rectangle txtEx = cn->textExtends(b->getText());
	return Dimension(txtEx.getWidth() + 35, txtEx.getHeight() + 15);
}
//-----------------------------------------------------------------------------
template <class ButtonModell>
bool FrxComponentUI<ButtonModell>::contains(AComponent::Ptr c,
		const Point2D &p)
{
	return Super::contains(c, p);
}

///////////////////////////////////////////////////////////////////////////////
}}}} // namespace(s)

#endif /* SAMBAG_FRXCOMPONENT_H */
