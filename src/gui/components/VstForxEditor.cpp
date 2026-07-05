#include <boost/bind.hpp>
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
#include <sambag/com/Config.h>
#include <sambag/com/Common.hpp>
#include <scripts/PluginScriptCtrl.hpp>
#include <filesystem>


extern void * __getHandlerForVstPlugins_(void *ptr);
extern void* hInstance;
namespace frx {

namespace processing {
    extern scripts::PluginScriptCtrl::Ptr
    getScriptControl(frx::gui::components::FrxCircuidViewPtr view);
}

namespace gui { namespace components {
//=============================================================================
//	Klasse VstForxEditor:
//=============================================================================
//-----------------------------------------------------------------------------
VstForxEditor::VstForxEditor (AudioEffect *aEff) : 
AEffEditor(aEff),
plug(NULL),
hiChamber("", FRX_ARCHIVE_VERSION)
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
	
	IFrxControl::NodeList entries, exits;
	getFrxControl(circ).createEntryExtitNodes(circ, entries, exits);
	
	double nodeRadius;
    double gap = 60.;
	sdcu::getUIManager().getProperty("Entry.radius", nodeRadius);
    sd::Coordinate xLoc = winSize.width()/2. - ((nodeRadius + gap) * entries.size())/2;
    sd::Coordinate yLoc = 0.;
    sd::Point2D viewPos = circ->getViewport()->getViewPosition();
    
    // entry nodes
    for (int i=0; i<entries.size(); ++i) {
        entries[i]->setLocation(xLoc + i * (nodeRadius + gap) + viewPos.x(), yLoc + viewPos.y());
    }
	xLoc = winSize.width()/2. - ((nodeRadius + gap) * exits.size())/2;
	//exit node
    for (int i=0; i<exits.size(); ++i) {
        yLoc = winSize.height() - nodeRadius*2 - 60.;
        exits[i]->setLocation(xLoc + i * (nodeRadius + gap) + viewPos.x(), yLoc + viewPos.y());
    }
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
	if (!view) {
		return;
	}
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
FrxCircuidView::Ptr VstForxEditor::deserializeViewTemp(::com::iArchive &ar, int version)
{
	FrxCircuidView::Ptr view;
	try {
		register_types(ar, version);
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
    sambag::disco::components::getWindowToolkit()->invokeLater(
        boost::bind(&VstForxEditor::loadInitScript, this)
    );
}
//-----------------------------------------------------------------------------
void VstForxEditor::loadInitScript() {
    SAMBAG_BEGIN_SYNCHRONIZED(mutex)
        std::string file = com::getSettings().getInitScriptFilename();
        try {
            scripts::PluginScriptCtrl::Ptr sctrl =
                frx::processing::getScriptControl(circView);
            if (!sctrl) {
                SAMBAG_LOG_WARN<<"get script control failed.";
                return;
            }
            if (!std::filesystem::exists(file)) {
                SAMBAG_LOG_INFO<<file<<" not found";
                return;
            }
            std::stringstream ss;
            std::filesystem::path luaPath(com::getSettings().getHomeDirectory());
            ss<<"package.path='"<<luaPath.generic_string()<<"/scripts/?.lua;' .. package.path";
            sctrl->execute(ss.str());
            sctrl->executeFile(file);
        } catch(const sambag::lua::ExecutionFailed &ex) {
            errorMessage("executing "+file+" failed: " + ex.errMsg);
        } catch(...) {
            errorMessage("executing "+file+" failed: unkown reason");
    }
    SAMBAG_END_SYNCHRONIZED
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
    SAMBAG_BEGIN_SYNCHRONIZED(mutex)
        if (circView) { // happens when view is deserialized while editor closed
            return circView;
        }

        FrxCircuidView::Ptr res;
        if (hiChamber.first.length()!=0) { //deserialize view
            std::stringstream ss;
            ss<<hiChamber.first;
            ::com::iArchive ar(ss);
            res = deserializeViewTemp(ar, hiChamber.second);
            hiChamber.first = "";
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
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
void VstForxEditor::onHostWindowOpen(void *src, const sdc::OnOpenEvent &ev)
{
	open(
        __getHandlerForVstPlugins_(clientWindow->getWindowImpl()->getSystemHandle())
    );
}
//-----------------------------------------------------------------------------
void VstForxEditor::open() {
	sdc::FramedWindow::Ptr fWin;
	if (!clientWindow) {
		clientWindow = fWin = sdc::FramedWindow::create();
		clientWindow->getContentPane()->setOpaque(false);
		clientWindow->getWindowImpl()->setFlag(sdc::WindowFlags::WND_RAW, true);
#if defined DISCO_USE_COCOA
        clientWindow->getWindowImpl()->setFlag(sdc::WindowFlags::WND_VST2X_CARBON_COCOA_HACK, true);
#endif
		clientWindow->addOnOpenEventListener(
			boost::bind(&VstForxEditor::onHostWindowOpen, this, _1, _2)
		);
	} else {
        fWin = boost::dynamic_pointer_cast<sdc::FramedWindow>(clientWindow);
    }
	clientWindow->setWindowBounds(
		sd::Rectangle(0,100,::com::getSettings().getWindowWidth(),
		::com::getSettings().getWindowHeight())
	);
    if (fWin) {
        fWin->setTitle("VSTForx " + com::getSettings().versionToString());
    }
	clientWindow->open();
}
//-----------------------------------------------------------------------------
void VstForxEditor::_open( void *ptr ) {
	if (isOpen()) {
		return;
	}
	using namespace sambag::com;
	using namespace sambag::disco;
	AEffEditor::open(ptr);
	sd::Dimension size = getEditorSize();
	sambag::disco::Rectangle bounds( 0, 0, size.width(), size.height() );
    try {
		SAMBAG_BEGIN_SYNCHRONIZED(mutex)
            SAMBAG_LOG_INFO<<"open view: ...";
			nestedWindow = createMainWindow(bounds);
			FrxCircuidViewPtr view = createView(nestedWindow);
			setCircuidView(view);
            SAMBAG_LOG_INFO<<"open view: SUCCEED";
		SAMBAG_END_SYNCHRONIZED
	} catch (const std::exception &ex) {
        SAMBAG_LOG_ERR<<"open view: FAILED, "<<ex.what();
		std::stringstream ss;
		ss<<"Could'nt create main view: "<<ex.what();
		errorMessage(ss.str());
		return;
	}
	catch (...) {
        SAMBAG_LOG_ERR<<"open view: FAILED, unkown error";
		std::stringstream ss;
		ss<<"Could'nt create main view: unkown error.";
		errorMessage(ss.str());
		return;
	}
	nestedWindow->validate();
    sce::EventSender<ViewIsReadyEvent>::notifyListeners(this, ViewIsReadyEvent());
	return;
}
//-----------------------------------------------------------------------------
bool VstForxEditor::open( void *ptr ) {
    if (isOpen()) {
        return true;
    }
    /*
     using cocoa: the editor appears but the parent window is invisble.
     It has something to do with the windowRef message on NSWindow.
     The window disappears right after calling this message.
     */
#if defined DISCO_USE_COCOA && defined SAMBAG_32
    sdc::getWindowToolkit()->invokeLater(
        boost::bind(&VstForxEditor::_open,this,ptr)
    );
#else
    _open(ptr);
#endif
    return true;
}
//-----------------------------------------------------------------------------
void VstForxEditor::close() {
	if (!isOpen() || !circView) {
		return;
	}
	AEffEditor::close();
	try {
		SAMBAG_BEGIN_SYNCHRONIZED(mutex)
		std::stringstream ss;
		::com::oArchive ar(ss);
		serializeViewTemp(ar, circView);
		hiChamber.first = ss.str();
        hiChamber.second = FRX_ARCHIVE_VERSION;
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
    nestedWindow->close();
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
