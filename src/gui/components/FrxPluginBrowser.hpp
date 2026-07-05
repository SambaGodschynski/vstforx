/*
 * FrxPluginBrowser.hpp
 *
 *  Created on: Tue Oct 30 14:30:15 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPLUGINBROWSER_H
#define SAMBAG_FRXPLUGINBROWSER_H

#include <memory>

#include "FrxProcessorBrowser.hpp"
#include <sambag/disco/components/events/ActionEvent.hpp>
#include <sambag/disco/components/Button.hpp>
#include <sambag/disco/components/Forward.hpp>
#include "FrxPluginBrowserCtrl.hpp"
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxPluginBrowser.
  */
class FrxPluginBrowser : public FrxProcessorBrowser {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<FrxPluginBrowser> Ptr;
	//-------------------------------------------------------------------------
	typedef std::weak_ptr<FrxPluginBrowser> WPtr;
	//-------------------------------------------------------------------------
	typedef FrxProcessorBrowser Super;
protected:
	//-------------------------------------------------------------------------
	FrxPluginBrowser(sdc::Window::Ptr parent=sdc::Window::Ptr()) :
		 FrxProcessorBrowser(parent) {}
	//-------------------------------------------------------------------------
	virtual void postConstructor();
public:
	//-------------------------------------------------------------------------
	sdc::ButtonPtr getBtnAdd() const {
		return btnAdd;
	}
	//-------------------------------------------------------------------------
	sdc::ButtonPtr getBtnClose() const {
		return btnClose;
	}
	//-------------------------------------------------------------------------
	static Ptr create( sdc::Window::Ptr parent=sdc::Window::Ptr() );
}; // FrxPluginBrowser
}}} // namespace(s)

#endif /* SAMBAG_PLUGINBROWSER_H */
