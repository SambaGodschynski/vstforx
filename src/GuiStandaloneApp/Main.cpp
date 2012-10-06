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
#include <gui/components/FrxConcreteProcessor.hpp>
#include <gui/components/FrxConcreteConnections.hpp>
#include <gui/components/FrxConcreteParameter.hpp>
#include <gui/components/FrxConcreteIO.hpp>
#include <gui/components/ui/FrxLookAndFeel.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <processing/IModelController.hpp>
#include <sambag/disco/svg/HtmlColors.hpp>
#include <stdlib.h>
#include <time.h>


#ifdef WIN32
#include <crtdbg.h>
//#include <vld.h>
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
		sd::ColorRGBA noneCol = HtmlColors::getColor("none");
		BOOST_FOREACH(HtmlColors::ColorMap::value_type v, cm) {
			if (i++ != rnd)
				continue;
			if (v.second == avoid || v.second == noneCol)
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
	
	enum { NUM = 3 };	
	fgc::FrxCircuidView::Ptr circ = fgc::FrxCircuidView::create();
	win->getContentPane()->add(circ);
	
	fgc::FrxComponent::Ptr comp = fgc::FrxEntryNode::create();
	comp->setLocation(370., 30.);
	circ->add(comp, fgc::FrxCircuidView::Z_IO);
/*
	for (int i=0; i<NUM; ++i) {
		comp = fgc::FrxStdKnob::create();
		int x = rand() % WIDTH;
		int y = rand() % HEIGHT;
		comp->setLocation(x, y);
		circ->add(comp, fgc::FrxCircuidView::Z_Knobs);
	}*/

	comp = fgc::FrxExitNode::create();
	comp->setLocation(370., 520.);
	circ->add(comp, fgc::FrxCircuidView::Z_IO);

	win->setWindowBounds(sambag::disco::Rectangle(100,100,WIDTH,HEIGHT));
	win->setTitle("VSTForx [D.I.S.C.O.]");
	win->validate();
	win->open();
	sdc::Window::startMainLoop();
	std::cout<<"bye dave."<<std::endl;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
namespace frx { namespace processing {
//-----------------------------------------------------------------------------
IModelController *
getModelController(frx::gui::components::FrxCircuidViewPtr view)
{
	return NULL;
}

}} // namespaced