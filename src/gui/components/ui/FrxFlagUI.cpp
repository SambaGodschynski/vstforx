/*
 * FrxFlagUI.cpp
 *
 *  Created on: Sun Dec 23 14:31:38 2012
 *      Author: Johannes Unger
 */

#include "FrxFlagUI.hpp"
#include <sambag/disco/components/ui/UIManager.hpp>
#include <sambag/disco/IDiscoFactory.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <boost/algorithm/string.hpp>
#include <gui/components/FrxNode.hpp>
#include <sambag/disco/FontCache.hpp>

namespace frx { namespace gui { namespace components { namespace ui {
//=============================================================================
//  Class FrxFlagUI
//=============================================================================
//-----------------------------------------------------------------------------
void FrxFlagUI::onRemovingTarget(void *, const OnRemoving &ev) {
	FrxFlag::Ptr flag = _flag.lock();
	if (!flag) {
		return;
	}
	FrxCircuidView::Ptr view = flag->getFirstContainer<FrxCircuidView>();
	if (!view) {
		return;
	}
	view->remove(flag);
}
//-----------------------------------------------------------------------------
void FrxFlagUI::postConstructor(Ptr self) {
	Super::postConstructor(self);
}
//-----------------------------------------------------------------------------
void FrxFlagUI::installDefaults(sdc::AComponentPtr c) {
	Super::installDefaults(c);
	FrxFlag::Ptr flag = boost::shared_dynamic_cast<FrxFlag>(c);
	SAMBAG_ASSERT(flag);
	_flag = flag;
	sdc::ui::UIManager &mg = sdc::ui::getUIManager();
	mg.getProperty("FrxFlag.style", flagStyle);
	sd::FontCache::instance().installFont( flagStyle.font() );
	FrxComponent::Ptr target = flag->getTarget();
	if (!target) {
		return;
	}
	distance = sd::Point2D(15., -30.); // TODO: accepts only (x>0, y<0)
	hGap = 3.;
	updateText();
}
//-----------------------------------------------------------------------------
sd::IDrawContext::Ptr FrxFlagUI::getOffscreenContext() {
	if (!offSf || !offCn) {
		sd::IDiscoFactory *fac = sd::getDiscoFactory();
		offSf = fac->createRecordingSurface();
		offCn = fac->createContext(offSf);
	}
	return offCn;
}
//-----------------------------------------------------------------------------
void FrxFlagUI::updateText() {
	using namespace sambag::disco;
	
	FrxFlag::Ptr flag = _flag.lock();
	if (!flag) {
		return;
	}
	FrxComponent::Ptr tg = flag->getTarget();
	if (!tg) {
		return;
	}

	IDrawContext::Ptr cn = getOffscreenContext();
	flagStyle.intoContext(cn);

	sd::FontCache &fc = sd::FontCache::instance();
	Rectangle ta = fc.getTextBounds( cn, tg->getUpperFlagText() );
	Rectangle tb = fc.getTextBounds( cn, tg->getLowerFlagText() );


	tb.width( std::max(ta.width(), tb.width()) );
	tb.height( ta.height() + tb.height() + hGap );

	flag->setSize(Dimension(
		tb.width() + ((distance.x()>0) ? (double)distance.x() : 
					 -1. * (double)distance.x()),
		tb.height() + ((distance.y()>0) ? (double)distance.y() : 
					   -1. * (double)distance.y())
	));
	sdc::AComponentPtr parent = flag->getParent();
	flag->redraw();
}
//-----------------------------------------------------------------------------
void FrxFlagUI::updateBounds(FrxFlag::Ptr flag) {
	sdc::AComponentPtr t = flag->getTarget();
	if (!t) {
		return;
	}
	sd::Rectangle r = t->getBounds();
	sd::Point2D m (
		r.x() + r.width()/2.,
		r.y() + r.height()/2.
	);
	flag->setLocation( sd::Point2D( // update flag position
		m.x(),
		-flag->getHeight() + m.y() 
	));
}
//-----------------------------------------------------------------------------
void FrxFlagUI::onTargetPropertyChanged(void *, const sce::PropertyChanged &ev)
{
	FrxFlag::Ptr flag = _flag.lock();
	if (!flag) {
		return;
	}

	if (ev.getPropertyName() == sdc::AComponent::PROPERTY_BOUNDS) {
		updateBounds(flag);
		return;
	}
	if (ev.getPropertyName() == FrxComponent::PROPERTY_FLAG_TXT) {
		std::string old, _new;
		ev.getOldValue(old);
		ev.getNewValue(_new);
		if (old!=_new) {
			updateText();
		}
	}

}
//-----------------------------------------------------------------------------
void FrxFlagUI::installTargetListeners(FrxComponent::Ptr c) {
	evcn = c->sce::EventSender<sce::PropertyChanged>::addTrackedEventListener(
		boost::bind(&FrxFlagUI::onTargetPropertyChanged, this, _1, _2),
		self
	);
	FrxComponent::Ptr fc = boost::shared_dynamic_cast<FrxComponent>(c);
	if (!fc) {
		return;
	}
	rmcn = fc->sce::EventSender<OnRemoving>::addTrackedEventListener(
		boost::bind(&FrxFlagUI::onRemovingTarget, this, _1, _2),
		self
	);
}
//-----------------------------------------------------------------------------
void FrxFlagUI::onFlagPropertyChanged(void *, const sce::PropertyChanged &ev)
{
	if (ev.getPropertyName() != FrxFlag::PROPERTY_TARGET) {
		return;
	}
	FrxComponent::Ptr old, _new;
	ev.getOldValue(old);
	ev.getNewValue(_new);
	if (old==_new) {
		return;
	}
	if (!_new) {
		return;
	}
	if (evcn.connected()) {
		evcn.disconnect();
	}
	if (rmcn.connected()) {
		rmcn.disconnect();
	}
	installTargetListeners(_new);
	updateText();

}
//-----------------------------------------------------------------------------
void FrxFlagUI::installListeners(sdc::AComponentPtr c) {
	Super::installListeners(c);
	firstDraw = true;
	FrxFlag::Ptr flag = boost::shared_dynamic_cast<FrxFlag>(c);
	SAMBAG_ASSERT(flag);
	flag->sce::EventSender<sce::PropertyChanged>::addTrackedEventListener(
		boost::bind(&FrxFlagUI::onFlagPropertyChanged, this, _1, _2),
		self
	);
	FrxComponent::Ptr target = flag->getTarget();
	if (!target) {
		return;
	}
	updateText();
	installTargetListeners(target);
}
//-----------------------------------------------------------------------------
FrxFlagUI::Ptr FrxFlagUI::create() {
	Ptr res( new FrxFlagUI() );
	res->self = res;
	res->postConstructor(res);
	return res;
} 
//-----------------------------------------------------------------------------
void FrxFlagUI::clip(FrxFlag::Ptr flag, 
	sd::IDrawContext::Ptr cn) const
{
	FrxNode::Ptr a = boost::shared_dynamic_cast<FrxNode>( flag->getTarget() );
	if (!a) {
		return;
	}

	sd::Point2D aLoc = a->getLocation();
	boost::geometry::add_point(aLoc, a->getPivot());
	
	boost::geometry::subtract_point(aLoc, flag->getLocation());
	
	cn->setFillRule(sd::IDrawContext::FILL_RULE_EVEN_ODD);
	
	cn->rect(sd::Rectangle(0, 0, flag->getWidth(), flag->getHeight()));
	cn->arc(aLoc, a->getRadius());
	cn->clip();
}
//-----------------------------------------------------------------------------
void FrxFlagUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	FrxFlag::Ptr flag = _flag.lock();
	if (firstDraw) {
		firstDraw = false;
		updateBounds(flag);
	}
	FrxComponent::Ptr target;
	if (!flag) {
		return;
	}
	target = flag->getTarget();
	if (!target) {
		return;
	}
	clip(flag, cn);
	sambag::com::Number fs = flagStyle.fontSize();
	flagStyle.intoContext(cn);
	
	sd::Rectangle r(0,0,c->getWidth(), c->getHeight());
	sd::FontCache &fc = sd::FontCache::instance();

	cn->moveTo(sd::Point2D(distance.x(), fs/2. - 2.));
	//cn->textPath(target->getUpperFlagText());
	fc.drawText(cn ,target->getUpperFlagText());
	cn->moveTo(sd::Point2D(distance.x(), fs+hGap + 2.));
	//cn->textPath(target->getLowerFlagText());
	fc.drawText(cn ,target->getLowerFlagText());
	cn->fill();

	cn->moveTo(sd::Point2D(0, r.height()));
	cn->lineTo(sd::Point2D(distance.x(), fs+hGap));
	cn->lineTo(sd::Point2D(r.width(), fs+hGap));
	cn->stroke();
}
}}}} // namespace(s)
