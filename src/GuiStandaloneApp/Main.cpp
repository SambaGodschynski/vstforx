/*
 * DiscoView.cpp
 *
 *  Created on: Mar 21, 2012
 *      Author: samba
 */

#include <sambag/disco/components/FramedWindow.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/timer/timer.hpp>
#include <assert.h>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/FrxPluginNode.hpp>
#include <gui/components/FrxConcreteConnections.hpp>
#include <gui/components/ui/FrxLookAndFeel.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <sambag/disco/svg/HtmlColors.hpp>
#include <stdlib.h>
#include <time.h>

#ifdef WIN32
#include <crtdbg.h>
#define SAMBAG_WINONLY(x) x
#else
#define SAMBAG_WINONLY(x)
#endif

namespace sd = sambag::disco;
namespace sdc = sambag::disco::components;

namespace {
	sd::ColorRGBA getRandomColor(const sd::ColorRGBA &avoid) {
		using namespace sd::svg;
		const HtmlColors::ColorMap &cm = HtmlColors::getColorMap();
		int rnd = rand() % cm.size();
		int i=0;
		BOOST_FOREACH(HtmlColors::ColorMap::value_type v, cm) {
			if (i++ != rnd)
				continue;
			if (v.second == avoid)
				return getRandomColor(avoid);
			return v.second;
		}
		return sd::ColorRGBA();
	}
	enum { WIDTH = 800, HEIGHT = 600 };
}

int main() {
	srand ( (int)time(NULL) );
	SAMBAG_WINONLY(
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); //VS memory tracking
	)
	using namespace sambag::disco;
	using namespace sambag::disco::components;
	namespace fgc = frx::gui::components;
	std::cout<<"hello dave."<<std::endl;
	FramedWindow::Ptr win = FramedWindow::create();
	ui::UIManager::instance().installLookAndFeel(win->getRootPane(),
		frx::gui::components::ui::FrxLookAndFeel::create()
	);
	
	enum { NUM = 150 };	fgc::FrxComponent::Ptr comps[NUM];
	fgc::FrxCircuidView::Ptr circ = fgc::FrxCircuidView::create();
	win->getContentPane()->add(circ);
	for (int i=0; i<NUM; ++i) {
		fgc::FrxComponent::Ptr comp = fgc::FrxPluginNode::create();
		comp->setSize(Dimension(50, 50));
		int x = rand() % WIDTH;
		int y = rand() % HEIGHT;
		comp->setLocation(x, y);
		comp->setForeground(getRandomColor(circ->getBackground()));
		circ->add(comp, fgc::FrxCircuidView::Z_ProcessorNodes);
		comps[i] = comp;
		if (i==0)
			continue;
		fgc::FrxConnection::Ptr con = fgc::IOCn::create();
		con->setComponentA( comps[i-1] );
		con->setComponentB( comps[i] );
		circ->add(con, fgc::FrxCircuidView::Z_Wires);
	}
	win->setWindowBounds(sambag::disco::Rectangle(100,100,WIDTH,HEIGHT));
	win->setTitle("VSTForx [D.I.S.C.O.]");
	win->validate();
	win->open();
	sdc::Window::startMainLoop();
	std::cout<<"bye dave."<<std::endl;
	return 0;
}

