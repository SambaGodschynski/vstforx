
#include "FrxSerializationRegister.hpp"
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
#include <gui/IFrxControl.hpp>
#include <OS_Specific/OS_com.h>
#include <sstream>
#include <com/Serialization.h>
#include <gui/FrxControl.hpp>
#include <processing/VstForxPlug.hpp>

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
FrxCircuidViewPtr VstForxEditor::createEmptyView() {
	FrxCircuidView::Ptr circ = FrxCircuidView::create();
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
void VstForxEditor::serializeView(std::ostream &os, FrxCircuidView::Ptr view) {
	try {
		::com::oArchive ar(os);
		RegisterFrxTypes::register_types(ar);
		getPlugin()->getViewModelMap()->lock(ar);
		FrxControl::serializeView(ar, view);
	} catch(const std::exception &ex) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			std::string("serialization of view failed: ") + ex.what()
		);
	} catch(...) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			"serialization of view failed."
		);
	}
}
//-----------------------------------------------------------------------------
FrxCircuidView::Ptr VstForxEditor::deserializeView(std::istream &is) {
	FrxCircuidView::Ptr view;
	try {
		::com::iArchive ar(is);
		RegisterFrxTypes::register_types(ar);
		getPlugin()->getViewModelMap()->unlock(ar);
		view = FrxControl::deserializeView(ar);
	} catch(const std::exception &ex) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			std::string("deserialization of view failed: ") + ex.what()
		);
	} catch(...) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			"deserialization of view failed."
		);
	}
	return view;
}
//-----------------------------------------------------------------------------
FrxCircuidViewPtr VstForxEditor::createView(sdc::Window::Ptr win) {
	FrxCircuidView::Ptr circ;
	if (bedroom.str().length()==0) {
		circ = createEmptyView();
		win->getContentPane()->add(circ);
		getPlugin()->registerView(circ);
		initEntryExit(circ);
	} else {
		circ = deserializeView(bedroom);
		win->getContentPane()->add(circ);
		getPlugin()->registerView(circ);
	}
	
	return circ;
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
		circView = createView(window);
	} catch (const std::exception &ex) {
		std::stringstream ss;
		ss<<"Could'nt create main view: "<<ex.what();
		errorMessage(ss.str());
		return false;
	}
	catch (...) {
		std::stringstream ss;
		ss<<"Could'nt create main view: unkown error.";
		errorMessage(ss.str());
		return false;
	}
	window->validate();
	return true;
}
//-----------------------------------------------------------------------------
void VstForxEditor::close() {
	AEffEditor::close();
	try {
		serializeView(bedroom, circView);
	} catch (const std::exception &ex) {
		std::stringstream ss;
		ss<<"closing main view failed: "<<ex.what();
		errorMessage(ss.str());
	} catch (...) {
		std::stringstream ss;
		ss<<"closing main view failed: unkown error.";
		errorMessage(ss.str());
	}
	getPlugin()->unRegisterView(circView);
}
//-----------------------------------------------------------------------------
bool VstForxEditor::getRect (ERect** rect) {
	*rect = &size;
	return true;
}
//-----------------------------------------------------------------------------
void VstForxEditor::message(const std::string &str) {
}
//-----------------------------------------------------------------------------
void VstForxEditor::warnMessage(const std::string &str) {
}
//-----------------------------------------------------------------------------
void VstForxEditor::errorMessage(const std::string &str) {
	::com::osMessageBox("Error", str, ::com::MSG_ALERT);
}
//-----------------------------------------------------------------------------
void VstForxEditor::idle() {
//	if (!window)
//		return;
//	window->invalidateWindow();
}
}}} // namespace(s)