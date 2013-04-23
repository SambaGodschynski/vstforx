
#include "FrxSerializationRegister.hpp"
#include "VstForxEditor.hpp"
#include <sambag/disco/components/WindowToolkit.hpp>
#include <sambag/com/ArbitraryType.hpp>
#include <sambag/com/Exception.hpp>
#include <sambag/disco/Geometry.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/FrxConcreteIO.hpp>
#include <sambag/disco/components/FramedWindow.hpp>
#include <gui/components/ui/FrxLookAndFeel.hpp>
#include <sambag/disco/components/BorderLayout.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <boost/tuple/tuple.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <gui/IFrxControl.hpp>
#include <OS_Specific/OS_com.h>
#include <OS_Specific/OS_gui.h>
#include <sstream>
#include <com/Settings.h>
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
	::com::Settings &set = com::getSettings();
	sd::Dimension winSize(set.getWindowWidth(), set.getWindowHeight());
	
	FrxNodePtr entry, exit;
	boost::tie(entry, exit) = getFrxControl(circ).createEntryExtitNodes(circ);
	
	if (!entry || !exit) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			"creating entry/exit failed."
		);
	}
	double nodeRadius;
	sdcu::getUIManager().getProperty("Entry.radius", nodeRadius);

	// entry node
	sd::Coordinate xLoc = winSize.width()/2. - nodeRadius;
	sd::Coordinate yLoc = 0.;
	sd::Point2D viewPos = circ->getViewport()->getViewPosition();
	entry->setLocation(xLoc + viewPos.x(), yLoc + viewPos.y());
	
	//exit node
	yLoc = winSize.height() - nodeRadius*2 - 60.;
	exit->setLocation(xLoc + viewPos.x(), yLoc + viewPos.y());
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
	ArbitraryType::Ptr pData = createObject(systemWindow);
	sdc::Window::Ptr win = getWindowToolkit()->createNestedWindow(pData, bounds);
	SAMBAG_ASSERT(win);
	sdc::ui::UIManager::instance().installLookAndFeel(win->getRootPane(),
		ui::FrxLookAndFeel::create()
	);
	return win;
}
//-----------------------------------------------------------------------------
void VstForxEditor::serializeViewTemp(::com::oArchive &ar, FrxCircuidView::Ptr view) {
	try {
		register_types(ar);
		getPlugin()->getViewModelMap()->lock(ar);
		FrxControl::serializeView(ar, view);
		FrxControl::serializeViewComponents(ar, view);
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
FrxCircuidView::Ptr VstForxEditor::deserializeViewTemp(::com::iArchive &ar) {
	FrxCircuidView::Ptr view;
	try {
		register_types(ar);
		getPlugin()->getViewModelMap()->unlock(ar);
		view = FrxControl::deserializeView(ar);
		getPlugin()->registerView(view);
		FrxControl::serializeViewComponents(ar, view);
	} catch(...) {
		return FrxCircuidView::Ptr();
	}
	return view;
}
//-----------------------------------------------------------------------------
void VstForxEditor::setCircuidView(FrxCircuidViewPtr view) {
	if (view == circView) {
		return;
	}
	FrxCircuidViewPtr old = circView;
	if (old) {
		old->close();
	}
	circView = view;
	if (circView) {
		circView->open();
	}

	if (nestedWindow) {
		if (old) { // remove old view
			nestedWindow->getContentPane()->remove(old);
		}
		nestedWindow->getContentPane()->add(circView, sdc::BorderLayout::CENTER, -1);
		nestedWindow->getContentPane()->validate();
		nestedWindow->getContentPane()->redraw();
	}
	if (!view) {
		return;
	}
	using namespace frx::processing;
	circView->setEditorResizeHandler(
		boost::bind(&VstForxEditor::setEditorSize, this, _1, _2)
	);
}
//-----------------------------------------------------------------------------
void VstForxEditor::setEditorSize(int width, int height) {
	if ( getPlugin()->requestEditorResize(width, height) ) {
		// operation succeed:
		return;
	}
	osHostWontResizeFix(nestedWindow, width, height);
}
//-----------------------------------------------------------------------------
FrxCircuidViewPtr VstForxEditor::createView(sdc::Window::Ptr win) {
	if (circView) { // happens when view is deserialized while editor closed
		return circView;
	}

	FrxCircuidView::Ptr res;
	if (hiChamber.length()!=0) { //deserialize view
		std::stringstream ss;
		ss<<hiChamber;
		::com::iArchive ar(ss);
		res = deserializeViewTemp(ar);
		hiChamber = "";
		if (res) {
			return res;
		}
	} 

	res = createEmptyView();
	getPlugin()->registerView(res);
	initEntryExit(res);
	if (!res) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			"view creation failed."
		);
	}
	return res;
}
//-----------------------------------------------------------------------------
void VstForxEditor::onHostWindowOpen(void *src, const sdc::OnOpenEvent &ev)
{
	open( clientWindow->getWindowImpl()->getSystemHandle() );
}
//-----------------------------------------------------------------------------
void VstForxEditor::open() {
	sdc::FramedWindow::Ptr fWin;
	if (!clientWindow) {
		clientWindow = fWin = sdc::FramedWindow::create();
		clientWindow->getContentPane()->setOpaque(false);
		clientWindow->getWindowImpl()->setFlag(sdc::WindowFlags::WND_RAW, true);
		clientWindow->addOnOpenEventListener(
			boost::bind(&VstForxEditor::onHostWindowOpen, this, _1, _2)
		);
	}
	clientWindow->setWindowBounds(
		sd::Rectangle(0,0,::com::getSettings().getWindowWidth(), 
		::com::getSettings().getWindowHeight())
	);
	fWin->setTitle("VSTForx " + com::getSettings().versionToString());
	clientWindow->open();
}
//-----------------------------------------------------------------------------
bool VstForxEditor::open( void *ptr ) {
	if (isOpen()) {
		return true;
	}
	using namespace sambag::com;
	using namespace sambag::disco;
	AEffEditor::open(ptr);
	sd::Dimension size = getEditorSize();
	sambag::disco::Rectangle bounds( 0, 0, size.width(), size.height() );
	try {
		SAMBAG_BEGIN_SYNCHRONIZED(mutex)
			nestedWindow = createMainWindow(bounds);
			FrxCircuidViewPtr view = createView(nestedWindow);
			setCircuidView(view);
		SAMBAG_END_SYNCHRONIZED
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
	nestedWindow->validate();
	return true;
}
//-----------------------------------------------------------------------------
void VstForxEditor::close() {
	if (!isOpen()) {
		return;
	}
	AEffEditor::close();
	try {
		SAMBAG_BEGIN_SYNCHRONIZED(mutex)
		std::stringstream ss;
		::com::oArchive ar(ss);
		serializeViewTemp(ar, circView);
		hiChamber = ss.str();
		SAMBAG_END_SYNCHRONIZED
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
	nestedWindow.reset();
	circView.reset();
	if (clientWindow) {
		clientWindow->close();
		//clientWindow.reset();
	}
}
//-----------------------------------------------------------------------------
sd::Dimension VstForxEditor::getEditorSize() const {
	::com::Settings &set = com::getSettings(); 
	return sd::Dimension(set.getWindowWidth(), set.getWindowHeight());
}
//-----------------------------------------------------------------------------
bool VstForxEditor::getRect (ERect** rect) {
	::com::Settings &set = com::getSettings(); 
	tmpRect.left = 0;
	tmpRect.top = 0;
	tmpRect.right = set.getWindowWidth();
	tmpRect.bottom = set.getWindowHeight();
	*rect = &tmpRect;
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
}
}}} // namespace(s)
