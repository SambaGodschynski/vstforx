/*
 * FrxScriptPluginEditor.cpp
 *
 *  Created on: Tue Nov  6 10:27:25 2012
 *      Author: Johannes Unger
 */

#include "FrxScriptPluginEditor.hpp"
#include <sambag/disco/components/windowImpl/WindowFlags.hpp>
#include <sambag/disco/components/WindowToolkit.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <gui/components/ui/FrxLookAndFeel.hpp>
#include <sambag/disco/components/BorderLayout.hpp>
#include <sambag/disco/components/GridLayout.hpp>
#include <sambag/disco/components/BoxLayout.hpp>
#include <sambag/disco/components/Panel.hpp>
#include <sambag/disco/components/ScrollPane.hpp>
#include <sambag/disco/components/TitledBorder.hpp>
#include <sambag/disco/components/SolidBorder.hpp>
#include <boost/algorithm/string.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxScriptPluginEditor
//=============================================================================
//-----------------------------------------------------------------------------
void FrxScriptPluginEditor::log(const std::string &msg) {
    sdc::getWindowToolkit()->invokeLater(
        boost::bind(&FrxScriptPluginEditor::delayedLog, this, msg),
        50, window
    );
}
//-----------------------------------------------------------------------------
void FrxScriptPluginEditor::delayedLog(const std::string &msg) {

    std::vector<std::string> res;
    boost::algorithm::split(res, msg, boost::algorithm::is_any_of("\n"));
    if (consoleList->ListModel::getSize()>MaxLog) {
        consoleList->removeRange(0, MaxLog/2);
    }
    
    BOOST_FOREACH(const std::string &x, res) {
        consoleList->addElement(x);
    }
    int i = consoleList->ListModel::getSize()-1;
    consoleList->ensureIndexIsVisible(i);
    consoleList->revalidate();
    consoleList->redraw();
    console->revalidate();
    console->getVerticalScrollBar()->setEnabled(true);
}
//-----------------------------------------------------------------------------
FrxScriptPluginEditor::FrxScriptPluginEditor(sdc::Window::Ptr parent, const sd::Dimension &size)
{
    createWindow(parent, size);
}
//-----------------------------------------------------------------------------
namespace {
    void __onClose(sdc::Window::WPtr _win) {
        sdc::Window::Ptr win = _win.lock();
        if (!win) {
            return;
        }
        win->close();
    }
}
void FrxScriptPluginEditor::createWindow(sdc::Window::Ptr parent, const sd::Dimension &size) {
    using namespace sambag::com;
	using namespace sambag::disco::components;
    void *ptr = parent->getWindowImpl()->getSystemHandle();
	ArbitraryType::Ptr pData = createObject(ptr);
	window = getWindowToolkit()->createNestedWindow(pData, size);
    parent->setName("HUBBA");
    SAMBAG_ASSERT(window);
    parent->addOnCloseEventListener(
        boost::bind(&__onClose, sdc::Window::WPtr(window))
    );
	sdc::ui::UIManager::instance().installLookAndFeel(window->getRootPane(),
		frx::gui::components::ui::FrxLookAndFeel::create()
	);
    sdc::AContainer::Ptr cnt = window->getContentPane();
    cnt->add(createConsolePane(), sdc::BorderLayout::CENTER, sdc::AContainer::APPEND);
	cnt->add(createBtnPane(), sdc::BorderLayout::SOUTH, sdc::AContainer::APPEND);
    cnt->revalidate();
}
//-----------------------------------------------------------------------------
sdc::AContainer::Ptr FrxScriptPluginEditor::createConsolePane() {
	consoleList = sdc::StringList::create();
	console = sdc::ScrollPane::create(consoleList);
	//console->setPreferredSize(sd::Dimension(730., 300.));
    console->setBorder(sdc::SolidBorder::create());
	return console;
}
//-----------------------------------------------------------------------------
sdc::AContainer::Ptr FrxScriptPluginEditor::createBtnPane() {
	sdc::Panel::Ptr pane = sdc::Panel::create();
    btnReload = sdc::Button::create();
    btnReload->setText("reload script");
    pane->add(btnReload);
    return pane;
}
//-----------------------------------------------------------------------------
FrxScriptPluginEditor::Ptr
FrxScriptPluginEditor::create(sdc::WindowPtr win, const sd::Dimension &size)
{
    Ptr res(new FrxScriptPluginEditor(win, size));
    return res;
}
//-----------------------------------------------------------------------------
FrxScriptPluginEditor::~FrxScriptPluginEditor() {
    window->close();
}
}}} // namespace(s)
