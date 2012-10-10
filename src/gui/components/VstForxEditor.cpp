
#include "VstForxEditor.hpp"
#include <sambag/disco/components/WindowToolkit.hpp>
#include <sambag/com/ArbitraryType.hpp>
#include <sambag/com/Exception.hpp>
#include <sambag/disco/Geometry.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/FrxConcreteIO.hpp>
#include <gui/components/ui/FrxLookAndFeel.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <boost/tuple/tuple.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <gui/FrxControl.hpp>
#include <OS_Specific/OS_com.h>
#include <sstream>
extern void* hInstance;
namespace frx { namespace gui { namespace components {
//=============================================================================
//	Klasse VstForxEditor:
//=============================================================================
//-----------------------------------------------------------------------------
VstForxEditor::VstForxEditor (AudioEffect *aEff) : 
AEffEditor(aEff),
plug(NULL)
{
	size.left = 0;
	size.top = 0;
	size.right = 800;
	size.bottom = 600;
}
//-----------------------------------------------------------------------------
VstForxEditor::~VstForxEditor () {
}
//-----------------------------------------------------------------------------
void VstForxEditor::setPlugin(frx::processing::VstForxPlug *plug) {
	this->plug = plug;
}
//-----------------------------------------------------------------------------
void VstForxEditor::initEntryExit(FrxCircuidViewPtr circ) {
	const sd::Dimension &winSize = circ->getParent()->getSize();
	
	FrxNodePtr entry, exit;
	boost::tie(entry, exit) = getFrxControl(circ).createEntryExtitNodes(circ);
	
	if (!entry || !exit) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			"creating entry/exit failed."
		);
	}

	// entry node
	sd::Coordinate xLoc = winSize.width()/2. - entry->getWidth()/2.;
	sd::Coordinate yLoc = 0.;
	entry->setLocation(xLoc, yLoc);
	
	//exit node
	xLoc = winSize.width()/2. - exit->getWidth()/2.;
	yLoc = winSize.height() - exit->getHeight();
	exit->setLocation(xLoc, yLoc);
}
//-----------------------------------------------------------------------------
FrxCircuidViewPtr VstForxEditor::createEmptyView(sdc::Window::Ptr win) {
	FrxCircuidView::Ptr circ = FrxCircuidView::create();
	win->getContentPane()->add(circ);
	return circ;
}
//-----------------------------------------------------------------------------
sdc::Window::Ptr VstForxEditor::createMainWindow(const sd::Rectangle &bounds) {
	using namespace sambag::com;
	using namespace sambag::disco::components;
	ArbitraryType::Ptr pData = createObject(
		std::make_pair(systemWindow, hInstance)
	);
	sdc::Window::Ptr win = 
		getWindowToolkit()->createNestedWindow(pData, bounds);
	SAMBAG_ASSERT(win);
	sdc::ui::UIManager::instance().installLookAndFeel(win->getRootPane(),
		ui::FrxLookAndFeel::create()
	);
	return win;
}
//-----------------------------------------------------------------------------
bool VstForxEditor::open( void *ptr ) {
	using namespace sambag::com;
	using namespace sambag::disco;
	AEffEditor::open(ptr);
	sambag::disco::Rectangle bounds(Point2D(size.left, size.top), 
		Point2D(size.right, size.bottom)
	);
	window = createMainWindow(bounds);
	try {
		circView = createEmptyView(window);
		getPlugin()->registerView(circView);
		initEntryExit(circView);
	} catch (const std::exception &ex) {
		std::stringstream ss;
		ss<<"Could'nt create main view: "<<ex.what();
		::com::MessageBox("Error", ss.str(), ::com::MSG_ALERT);
		return false;
	}
	catch (...) {
		std::stringstream ss;
		ss<<"Could'nt create main view: unkown error.";
		::com::MessageBox("Error", ss.str(), ::com::MSG_ALERT);
		return false;
	}
	window->validate();
	return true;
}
//-----------------------------------------------------------------------------
void VstForxEditor::close() {
	AEffEditor::close();
	getPlugin()->unRegisterView(circView);
}
//-----------------------------------------------------------------------------
bool VstForxEditor::getRect (ERect** rect) {
	*rect = &size;
	return true;
}
//-----------------------------------------------------------------------------
void VstForxEditor::idle() {
/*	if (!window)
		return;
	window->invalidateWindow();*/
}
}}} // namespace(s)



