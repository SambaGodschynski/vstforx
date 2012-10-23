/*
 * FrxColumnBrowser.hpp
 *
 *  Created on: Mon Oct 22 11:19:25 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCOLUMNBROWSER_H
#define SAMBAG_FRXCOLUMNBROWSER_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/disco/components/FramedWindow.hpp>
#include <sambag/disco/components/ColumnBrowser.hpp>
#include <sambag/disco/components/Forward.hpp>
#include <gui/components/ui/FrxLookAndFeel.hpp>
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <sambag/disco/components/BorderLayout.hpp>
#include <sambag/disco/components/Panel.hpp>

namespace frx { namespace gui { namespace components {
namespace sce = sambag::com::events;
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxColumnBrowser.
  */
template <class T>
class FrxColumnBrowser : public sdc::FramedWindow {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxColumnBrowser> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxColumnBrowser> WPtr;
	//-------------------------------------------------------------------------
	sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const;
	//-------------------------------------------------------------------------
	typedef sdc::ColumnBrowser<T> BrowserImpl;
	//-------------------------------------------------------------------------
	typedef sdc::FramedWindow Super;
protected:
	//-------------------------------------------------------------------------
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	FrxColumnBrowser( sdc::Window::Ptr parent=sdc::Window::Ptr() ) :
		sdc::FramedWindow(parent) {}
	//-------------------------------------------------------------------------
private:
	//-------------------------------------------------------------------------
	sdc::AContainerPtr buttonPane;
	//-------------------------------------------------------------------------
	typename BrowserImpl::Ptr browser;
public:
	//-------------------------------------------------------------------------
	sdc::AContainerPtr getButtonPane() const {
		return buttonPane;
	}
	//-------------------------------------------------------------------------
	typename BrowserImpl::Ptr getBrowserImpl() const {
		return browser;
	}
}; // FrxColumnBrowser
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class T>
sdcu::AComponentUIPtr 
FrxColumnBrowser<T>::createComponentUI(sdcu::ALookAndFeelPtr laf) const
{
	return laf->getUI<FrxColumnBrowser>();
}
//-----------------------------------------------------------------------------
template <class T>
void FrxColumnBrowser<T>::postConstructor() {
	Super::postConstructor();
	sdc::ui::UIManager::instance().installLookAndFeel(getRootPane(),
		ui::FrxLookAndFeel::create()
	);
	browser = typename BrowserImpl::create();
	getContentPane()->add(browser);
	buttonPane = sdc::Panel::create();
	getContentPane()->add(buttonPane, sdc::BorderLayout::SOUTH, -1);
}
}}} // namespace(s)

#endif /* SAMBAG_FRXCOLUMNBROWSER_H */
