/*
 * About.cpp
 *
 *  Created on: Thu Jul 11 14:51:19 2013
 *      Author: Johannes Unger
 */

#include "About.hpp"
#include <sambag/disco/components/ui/UIManager.hpp>
#include "ui/FrxLookAndFeel.hpp"
#include <sambag/disco/components/BoxLayout.hpp>
#include <sambag/disco/components/Label.hpp>
#include <boost/foreach.hpp>
#include <boost/assign.hpp>
#include <sambag/disco/IResourceManager.hpp>
#include <sambag/disco/components/ComponentWrapper.hpp>
#include <sambag/disco/svg/graphicElements/Image.hpp>
#include <com/Settings.h>
#include <sambag/disco/IDiscoFactory.hpp>
#include <sambag/disco/components/Timer.hpp>
#include <math.h>

#include <boost/version.hpp>
#include <cairo-version.h>
#include <sqlite3.h>

namespace frx { namespace gui { namespace components {
namespace {
//-----------------------------------------------------------------------------
class AboutLabel : public sdc::Label {
public:
	typedef boost::shared_ptr<AboutLabel> Ptr;
	typedef sdc::Label Super;
protected:
	AboutLabel(){
		setOpaque(false);
		setAlignmentX(0.5);
		setAlignmentY(0.5);
	}
public:
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(AboutLabel)
	virtual sd::Dimension getPreferredSize() {
		sd::Dimension sz = Super::getMinimumSize();
		sz.height( sz.height() + 5. );
		sz.width( sz.width() + 5. );
		return sz; 
	}
	void drawComponent(sd::IDrawContext::Ptr cn) {
		Super::drawComponent(cn);
	}
};
//-----------------------------------------------------------------------------
void onBgUpdate(void*, const sdc::TimerEvent &ev, About::WPtr c) {
	using namespace boost::numeric::ublas;
	About::Ptr about = c.lock();
	if (!about) {
		return;
	}
	if (about->getComponentCount() == 0) {
		return;
	}
	double t = about->getClock().elapsed().wall / 1000000.;
	sdc::AComponent::Ptr view = about->getContentPane()->getComponent(0);
	sd::IPattern::Ptr bg = view->getBackgroundPattern();
	//sambag::math::Matrix m = bg->getMatrix();
	double tmp = t * 0.01;
	sambag::math::Matrix m = sambag::math::translate2D(t*0.01, t*0.01);
	tmp = sin(t * 0.000001) * 3.;
	m = prod(m , sambag::math::scale2D(tmp, tmp));
	tmp = sin(t * 0.00001) * 45.;
	m = prod(m , sambag::math::rotate2D(tmp));
	bg->setMatrix(m);
	view->redraw();
}
} // namespace(s)
//=============================================================================
//  Class About
//=============================================================================
//-----------------------------------------------------------------------------
void About::postConstructor() {
	Super::postConstructor();
	sdc::ui::UIManager::instance().installLookAndFeel(getRootPane(),
		ui::FrxLookAndFeel::create()
	);
	setWindowSize(sd::Dimension(320., 240.));
	windowImpl->setFlag(sdc::WindowFlags::WND_ALWAYS_ON_TOP, true);
	windowImpl->setFlag(sdc::WindowFlags::WND_RESIZEABLE, false);

	initView();
}
//-----------------------------------------------------------------------------
void About::getTextLines( Lines &out ) {
	std::stringstream ss;
	ss << BOOST_VERSION / 100000 << "." << BOOST_VERSION / 100 % 1000;
	std::string boost_version = ss.str();
	ss.str("");
	ss << CAIRO_VERSION_MAJOR << "." << CAIRO_VERSION_MINOR << "." << CAIRO_VERSION_MICRO;
	std::string cairo_version = ss.str();
	ss.str("");
	ss << SQLITE_VERSION;
	std::string sqll_version = ss.str();
	out = boost::assign::list_of
		( com::getSettings().versionToString() )
		("www.vstforx.de")
		("(c)Samba Godschynski")
		("external libs:")
		("boost-"+boost_version)
		("cairo-"+cairo_version)
		("sqlite-"+sqll_version)
		("loki-0.1.7");
}
//-----------------------------------------------------------------------------
void About::onMouseEvent(void *src, const sdce::MouseEvent &ev) {
	if (ev.getType() == sdce::MouseEvent::DISCO_MOUSE_CLICKED) {
		close();
	}
}
//-----------------------------------------------------------------------------
void About::initView() {
	sdc::AContainer::Ptr view = sdc::Panel::create();
	//view->setLayout( sdc::BoxLayout::create(view, sdc::BoxLayout::PAGE_AXIS ) );
	
	sd::ISurface::Ptr logo = 
		sd::getResourceManager().getImage("About.logo");
	sd::ISurface::Ptr bg = 
		sd::getResourceManager().getImage("About.bg");
	
	if (logo) {
		typedef sdc::ComponentWrapper<sdsg::Image> Image;
		Image::Ptr img = Image::create();
		img->setMouseEventsEnabled(false);
		img->getObject()->getOutline().x0().x().setType(sds::units::Unit::PERCENT);
		img->getObject()->getOutline().x0().x().setValue(50);
		img->getObject()->getOutline().x0().y().setType(sds::units::Unit::PERCENT);
		img->getObject()->getOutline().x0().y().setValue(10);

		img->getObject()->getOutline().size().width().setType(sds::units::Unit::PX);
		img->getObject()->getOutline().size().width().setValue(130);
		img->getObject()->getOutline().size().height().setType(sds::units::Unit::PX);
		img->getObject()->getOutline().size().height().setValue(130);
		img->getObject()->setSurface(logo);
		img->updateBounds();
		img->setPreferredSize(sd::Dimension(200, 150));
		//img->setAlignmentX(1);
		//img->setAlignmentY(1);
		view->add(img);
	}

	if (bg) {
		sd::ISurfacePattern::Ptr pt = sd::getDiscoFactory()->createSurfacePattern(bg);
		pt->setExtendType(sd::IPattern::DISCO_EXTEND_REPEAT);
		view->setBackground(pt);
		sdc::Timer::Ptr timer = sdc::Timer::create(50);
		timer->setNumRepetitions(-1);
		timer->sce::EventSender<sdc::TimerEvent>::addTrackedEventListener(
			boost::bind(&onBgUpdate, _1, _2, WPtr(getPtr())),
			getPtr()
		);
		timer->start();
	}
	
	Lines lines;
	getTextLines(lines);
	BOOST_FOREACH( std::string &txt, lines) {
		sdc::Label::Ptr l = AboutLabel::create();
		l->setForeground(sd::ColorRGBA(1,1,1));
		l->setMouseEventsEnabled(false);
		l->setFont( sd::Font("arial", 12., sd::Font::SLANT_NORMAL, sd::Font::WEIGHT_BOLD) );
		l->setText(txt);
		view->add(l);
	} 
	getContentPane()->add(view);

	view->sce::EventSender<sdce::MouseEvent>::addEventListener(
		boost::bind(&About::onMouseEvent, this, _1, _2)
	);
}
}}} // namespace(s)
