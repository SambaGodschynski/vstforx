/*
 * DiscoView.cpp
 *
 *  Created on: Mar 21, 2012
 *      Author: samba
 */

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.VC90.CRT' version='9.0.21022.8' processorArchitecture='X86' publicKeyToken='1fc8b3b9a1e18e3b' language='*'\"")

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
#include <processing/IHostInfo.h>
#include <processing/graph.h>
#include <processing/ModelController.hpp>
#include <gui/ViewModelMap.hpp>
#include <sambag/disco/components/Timer.hpp>
#include <sambag/disco/components/WindowToolkit.hpp>
#include <math.h>
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
		fgc::register_types(ar);
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
	fgc::register_types(ar);
	fg::FrxControl::serializeView(ar, view);
	f.close();
}

fgc::FrxCircuidView::Ptr createNewView(sdc::Window::Ptr win) {
	enum { NUM = 3 };	
	fgc::FrxCircuidView::Ptr circ = fgc::FrxCircuidView::create();
	fgc::FrxComponent::Ptr comp = fgc::FrxEntryNode::create();
	comp->setLocation(370., 30.);
	circ->add(comp, fgc::FrxCircuidView::Z_IO, true);

	comp = fgc::FrxExitNode::create();
	comp->setLocation(370., 520.);
	circ->add(comp, fgc::FrxCircuidView::Z_IO, true);
	return circ;
}

sdc::Timer::Ptr ptimer;


void onTimer(void *src, const sdc::TimerEvent &ev) {
	static float t = 0.f;
	static const float maxt = 100000.f;
	frx::processing::IModelController::Ptr ctrl = 
		frx::processing::getModelController(fgc::FrxCircuidViewPtr());
	int numP = 10;//ctrl->getNumHostParameter();
	for (int i=0; i<numP; ++i) {
		//float value = (float)(rand() % RAND_MAX) / (float)RAND_MAX;
		float value = 
			abs(sin( t + (float)i/(float)numP));
		ctrl->getHostParameter(i)->setValue(value);
	}
	t+=0.1;
	if (t>maxt)
		t = t-maxt;
}

void initParameterTimer() {
	ptimer = sdc::Timer::create(10);
	ptimer->setNumRepetitions(-1);
	ptimer->EventSender<sdc::TimerEvent>::addEventListener(&onTimer);
	ptimer->start();
}

int main() {
	const std::string savefile("frxview.save");
	// init settings
	::com::initSettings(".");
	initParameterTimer();
	srand ( (int)time(NULL) );
	SAMBAG_WINONLY(
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); //VS memory tracking
	)
	using namespace sambag::disco;
	using namespace sambag::disco::components;
	namespace fgc = frx::gui::components;
	std::cout<<"hello dave."<<std::endl;
	FramedWindow::Ptr win = FramedWindow::create();
	try {
		ui::UIManager::instance().installLookAndFeel(win->getRootPane(),
			frx::gui::components::ui::FrxLookAndFeel::create()
		);
	} catch (const std::exception &ex) {
		std::cout<<ex.what()<<std::endl;
		return 0;
	}


	fgc::FrxCircuidView::Ptr circ = deserializeView(savefile);
	if (!circ) {
		circ = createNewView(win);
	}

	win->getContentPane()->add(circ);
	win->setWindowBounds(sambag::disco::Rectangle(100,100,WIDTH,HEIGHT));
	win->setTitle("VSTForx [D.I.S.C.O.]");
	//win->validate();
	win->open();
	
	sdc::Window::startMainLoop();
	ptimer->stop();

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
	static ::processing::Graph::Ptr graph;
	static ModelController::Ptr ctrl;
	if (!ctrl) {
		//hostInfoAdapter = IHostInfo::Ptr(new HostInfoAdapter(*this));
		graph = ::processing::Graph::create(IHostInfo::Ptr());
		ctrl = ModelController::create();
		ctrl->setGraph(graph);
		int num = graph->getNumHostParameter();
		for (int i=0; i<num; ++i) {
			graph->getHostParameter(i)->setValue((float)(i)/(float)num);
		}
	}
	return ctrl;
}
}} // namespace(s)

namespace frx { namespace gui {
//-----------------------------------------------------------------------------
IViewModelMap::Ptr 
getViewModelMap(components::FrxCircuidViewPtr view)
{
	static frx::gui::ViewModelMap::Ptr _map;
	if (!_map) {
		_map = frx::gui::ViewModelMap::create();
	}
	return _map;
}
}} // namespaces