/*
 * FrxScriptPluginEditor.hpp
 *
 *  Created on: Tue Nov  6 10:27:25 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FrxScriptPluginEditor_H
#define SAMBAG_FrxScriptPluginEditor_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/disco/components/Window.hpp>
#include <gui/HandyNamespaces.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <sambag/disco/components/List.hpp>
#include <sambag/disco/components/ScrollPane.hpp>
#include <sambag/disco/components/Button.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxScriptPluginEditor.
  */
class FrxScriptPluginEditor :
    public boost::enable_shared_from_this<FrxScriptPluginEditor>
{
//=============================================================================
public:
    //-------------------------------------------------------------------------
    enum {MaxLog=500};
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxScriptPluginEditor> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxScriptPluginEditor> WPtr;
protected:
    //-------------------------------------------------------------------------
    void createWindow(void *ptr, const sd::Dimension &size);
	//-------------------------------------------------------------------------
    FrxScriptPluginEditor(void *ptr, const sd::Dimension &size);
    //-------------------------------------------------------------------------
    sdc::Window::Ptr window;
    //-------------------------------------------------------------------------
    sdc::AContainer::Ptr createConsolePane();
    //-------------------------------------------------------------------------
    sdc::AContainer::Ptr createBtnPane();
    //-------------------------------------------------------------------------
    void delayedLog(const std::string &msg);
private:
    //-------------------------------------------------------------------------
    sdc::ScrollPane::Ptr console;
    //-------------------------------------------------------------------------
    sdc::StringList::Ptr consoleList;
    //-------------------------------------------------------------------------
    sdc::Button::Ptr btnReload;
public:
    //-------------------------------------------------------------------------
    sdc::Button::Ptr getReloadButton() const {
        return btnReload;
    }
    //-------------------------------------------------------------------------
    virtual ~FrxScriptPluginEditor();
    //-------------------------------------------------------------------------
    void log(const std::string &msg);
    //-------------------------------------------------------------------------
    sdc::Window::Ptr getWindow() const {
        return window;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief creates nested window using ptr as parent window
     * @see as in vstsdk2.4 VstPluginEditor::Open
     */
    static Ptr create(void *ptr, const sd::Dimension &size);
    
}; // FrxScriptPluginEditor
}}} // namespace(s)

#endif /* SAMBAG_FrxScriptPluginEditor_H */
