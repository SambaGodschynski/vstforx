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
	
	FrxComponent::Ptr target = flag->getTarget();
	if (!target) {
		return;
	}
	distance = sd::Point2D(15., -30.); // TODO: accepts only (x>0, y<0)
	hGap = 3.;
	updateText(target->getFlagText());
}
//-----------------------------------------------------------------------------
void FrxFlagUI::updateText(const std::string &txt) {
	using namespace sambag::disco;
	std::vector<std::string> strs;
	strs.reserve(2);
	boost::split(strs, txt, boost::is_any_of("/"));
	if (strs.size() >= 1) {
		upper = strs[0];
	}
	if (strs.size() >= 2) {
		lower = strs[1];
	}
	
	IDiscoFactory *fac = getDiscoFactory();
	IRecordingSurface::Ptr sf = fac->createRecordingSurface();
	IDrawContext::Ptr cn = fac->createContext(sf);
	flagStyle.intoContext(cn);
	
	Rectangle ta = cn->textExtends(upper);
	Rectangle tb = cn->textExtends(lower);
	tb.width( std::max(ta.width(), tb.width()) );
	tb.height( ta.height() + tb.height() + hGap );
	
	FrxFlag::Ptr flag = _flag.lock();
	if (!flag) {
		return;
	}
	flag->setSize(Dimension(
		tb.width() + ((distance.x()>0) ? distance.x() : -1. * distance.x()),
		tb.height() + ((distance.y()>0) ? distance.y() : -1. * distance.y())
	));
	sdc::AComponentPtr parent = flag->getParent();
	if (parent) {
		parent->redraw();
	}
}
//-----------------------------------------------------------------------------
void FrxFlagUI::onTargetPropertyChanged(void *, const sce::PropertyChanged &ev)
{
	FrxFlag::Ptr flag = _flag.lock();
	if (!flag) {
		return;
	}

	if (ev.getPropertyName() == sdc::AComponent::PROPERTY_BOUNDS) {
		sd::Rectangle r;
		ev.getNewValue(r);
		sd::Point2D m (
			r.x() + r.width()/2.,
			r.y() + r.height()/2.
		);
		flag->setLocation( sd::Point2D( // update flag position
			m.x(),
			-flag->getHeight() + m.y() 
		));
		return;
	}
	if (ev.getPropertyName() == FrxComponent::PROPERTY_FLAG_TXT) {
		std::string old, _new;
		ev.getOldValue(old);
		ev.getNewValue(_new);
		if (old!=_new) {
			updateText(_new);
		}
	}

}
//-----------------------------------------------------------------------------
void FrxFlagUI::installTargetListeners(FrxComponent::Ptr c) {
	evcn = c->EventSender<sce::PropertyChanged>::addTrackedEventListener(
		boost::bind(&FrxFlagUI::onTargetPropertyChanged, this, _1, _2),
		self
	);
	FrxComponent::Ptr fc = boost::shared_dynamic_cast<FrxComponent>(c);
	if (!fc) {
		return;
	}
	rmcn = fc->EventSender<OnRemoving>::addTrackedEventListener(
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
	updateText(_new->getFlagText());
}
//-----------------------------------------------------------------------------
void FrxFlagUI::installListeners(sdc::AComponentPtr c) {
	Super::installListeners(c);
	FrxFlag::Ptr flag = boost::shared_dynamic_cast<FrxFlag>(c);
	SAMBAG_ASSERT(flag);
	flag->EventSender<sce::PropertyChanged>::addTrackedEventListener(
		boost::bind(&FrxFlagUI::onFlagPropertyChanged, this, _1, _2),
		self
	);
	FrxComponent::Ptr target = flag->getTarget();
	if (!target) {
		return;
	}
	installTargetListeners(target);
}
//-----------------------------------------------------------------------------
FrxFlagUI::Ptr FrxFlagUI::create() {
	Ptr res( new FrxFlagUI() );
	res->self = res;
	return res;
} 
//-----------------------------------------------------------------------------
void FrxFlagUI::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	FrxFlag::Ptr flag = _flag.lock();
	FrxComponent::Ptr target;
	if (!flag) {
		return;
	}
	target = flag->getTarget();
	if (!target) {
		return;
	}
	sambag::com::Number fs = flagStyle.fontSize();
	flagStyle.intoContext(cn);
	sd::Rectangle r = cn->clipExtends();
	cn->moveTo(sd::Point2D(distance.x(), fs));
	cn->textPath(upper);
	cn->moveTo(sd::Point2D(distance.x(), 2*fs+hGap));
	cn->textPath(lower);
	cn->fill();

	cn->moveTo(sd::Point2D(0, r.height()));
	cn->lineTo(sd::Point2D(distance.x(), fs+hGap));
	cn->lineTo(sd::Point2D(r.width(), fs+hGap));
	cn->stroke();
}
}}}} // namespace(s)
