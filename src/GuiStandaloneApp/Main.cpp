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
#include <gui/components/ui/FrxLookAndFeel.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>

#ifdef WIN32
#include <crtdbg.h>
#define SAMBAG_WINONLY(x) x
#else
#define SAMBAG_WINONLY(x)
#endif

namespace sd = sambag::disco;
namespace sdc = sambag::disco::components;

int main() {
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
	win->setWindowBounds(sambag::disco::Rectangle(100,100,230,200));
	win->setTitle("VSTForx [D.I.S.C.O.]");


	fgc::FrxCircuidView::Ptr circ = fgc::FrxCircuidView::create();
	win->getContentPane()->add(circ);
	fgc::FrxComponent::Ptr comp = fgc::FrxPluginNode::create();
	comp->setSize(Dimension(50, 50));
	comp->setLocation(10, 10);
	circ->add(comp);


	win->validate();
	win->open();
	sdc::Window::startMainLoop();
	std::cout<<"bye dave."<<std::endl;
	return 0;
}

