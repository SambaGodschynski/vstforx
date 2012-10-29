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
#include <com/Serialization.h>
#include <gui/FrxControl.hpp>
#include <gui/components/FrxConcreteProcessor.hpp>
#include <gui/components/FrxConcreteConnections.hpp>
#include <gui/components/FrxConcreteParameter.hpp>
#include <gui/components/FrxConcreteIO.hpp>
#include <gui/components/ui/FrxLookAndFeel.hpp>
#include <gui/IViewModelMap.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <processing/IModelController.hpp>
#include <sambag/disco/svg/HtmlColors.hpp>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <gui/components/FrxSerializationRegister.hpp>
#include <gui/components/FrxColumnBrowser.hpp>
#include <com/Settings.h>

#ifdef WIN32
#include <crtdbg.h>
//#include <vld.h>
#define SAMBAG_WINONLY(x) x
#else
#define SAMBAG_WINONLY(x)
#endif

namespace sd = sambag::disco;
namespace sdc = sambag::disco::components;
namespace fg = frx::gui;
namespace fgc = fg::components;

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

fgc::FrxCircuidView::Ptr deserializeView(const std::string &file) {
	std::fstream f(file.c_str(), std::ios_base::in);
	if (f.fail()) {
		std::cout<<"could'nt open: "<<file<<std::endl;
		return fgc::FrxCircuidView::Ptr();
	}
	fgc::FrxCircuidView::Ptr view;
	try {
		::com::iArchive ar(f);
		fgc::RegisterFrxTypes::register_types(ar);
		view = 
			fg::FrxControl::deserializeView(ar);
	} catch(...) {
		std::cout<<"could'nt deserialize: "<<file<<std::endl;
		return fgc::FrxCircuidView::Ptr();
	}
	f.close();
	return view;
}

void serializeView(const std::string &file, fgc::FrxCircuidView::Ptr view) {
	std::fstream f(file.c_str(), std::ios_base::out | std::ios_base::trunc);
	SAMBAG_ASSERT(!f.fail());
	::com::oArchive ar(f);
	fgc::RegisterFrxTypes::register_types(ar);
	fg::FrxControl::serializeView(ar, view);
	f.close();
}

fgc::FrxCircuidView::Ptr createNewView(sdc::Window::Ptr win) {
	enum { NUM = 3 };	
	fgc::FrxCircuidView::Ptr circ = fgc::FrxCircuidView::create();
	fgc::FrxComponent::Ptr comp = fgc::FrxEntryNode::create();
	comp->setLocation(370., 30.);
	circ->add(comp, fgc::FrxCircuidView::Z_IO);

	comp = fgc::FrxExitNode::create();
	comp->setLocation(370., 520.);
	circ->add(comp, fgc::FrxCircuidView::Z_IO);
	return circ;
}

int main() {
	const std::string savefile("frxview.save");
	// init settings
	::com::initSettings("./");
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


	fgc::FrxCircuidView::Ptr circ = deserializeView(savefile);
	if (!circ) {
		circ = createNewView(win);
	}

	win->getContentPane()->add(circ);
	win->setWindowBounds(sambag::disco::Rectangle(100,100,WIDTH,HEIGHT));
	win->setTitle("VSTForx [D.I.S.C.O.]");
	win->validate();
	win->open();
	
	sdc::Window::startMainLoop();

	serializeView(savefile, circ);
	std::cout<<"bye dave."<<std::endl;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
namespace frx { namespace processing {
//-----------------------------------------------------------------------------
IModelController::Ptr
getModelController(frx::gui::components::FrxCircuidViewPtr view)
{
	return IModelController::Ptr();
}
}} // namespaces
namespace frx { namespace gui {
//-----------------------------------------------------------------------------
IViewModelMap::Ptr 
getViewModelMap(components::FrxCircuidViewPtr view) {
	return IViewModelMap::Ptr();
}
}} // namespaces