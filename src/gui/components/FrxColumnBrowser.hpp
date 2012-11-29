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
#include <boost/function.hpp>
#include <string>
#include "IFrxColumnBrowserCtrl.hpp"
#include "Forward.hpp"
#include <gui/components/FrxParameterLabel.hpp>
#include <sambag/com/ArbitraryType.hpp>

namespace frx { namespace gui { namespace components {
namespace sce = sambag::com::events;
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
//=============================================================================
// @class BrowserConstants
struct BrowserConstants {
//=============================================================================
	static const std::string FRX_BROWSER_FOLDER; 
	static const std::string FRX_BROWSER_DEFAULT;
	static const std::string FRX_BROWSER_PLUGIN;
	static const std::string FRX_BROWSER_PROCESSOR;
	static const std::string FRX_BROWSER_PARAMETER; 
	static sd::ISurface::Ptr getIcon(const std::string &type);
};
//=============================================================================
/**
 * @class FrxBrowser content.
 * The elements which are contained by a browser class.
 * Each object has the informations: - a string representation
 *									 - a function callback initiated if object is
 *                                     acceppted by user action (add/ok/..).
 *                                   - some callback functions to support
 *                                     parameter value changing in browser,
 *                                     because the default list impl. (rubber
 *                                     stamp rendering) dosent support component
 *                                     mouse event processing.
 */
struct BrowserNode : public BrowserConstants {
//=============================================================================
	std::string name;
	/**
	 * specific draw callback: will be called with renderer 
	 * component before rendering.
	 */
	typedef boost::function<void(sdc::AComponentPtr)> 
		DrawCallback;
	DrawCallback drawCallback;
	/**
	 * value changed callback.
	 * called by FrxBrowserListUI when parameter changed in browser.
	 */
	typedef boost::function<void(float)> ValueChanged;
	ValueChanged valueChanged;
	/**
	 * will be called when node is selected and (eg.) ok is pressed.
	 */ 
	typedef sambag::com::ArbitraryType Result;
	typedef Result::Ptr ResultPtr;
	typedef boost::function<ResultPtr()> AcceptedFunction;
	AcceptedFunction f;
	std::string type; // specify node type for rendering 
	BrowserNode(const std::string &name, bool isFolder = false,
		AcceptedFunction &f = AcceptedFunction()
	) : name(name), f(f)
	{
		type = isFolder ? FRX_BROWSER_FOLDER : FRX_BROWSER_DEFAULT;
	}
	BrowserNode(const std::string &name, const std::string &type,
		AcceptedFunction &f = AcceptedFunction()
	) : name(name), f(f), type(type)
	{
	}
	BrowserNode(const char *name = "") : name(name), 
		type(FRX_BROWSER_DEFAULT)
	{
	}
	bool operator==(const BrowserNode &n) const { 
		return name==n.name && type==n.type
			&& &f == &(n.f); // boost::functions are incomparable
	}
	ResultPtr accept() const {
		if (f)
			return f();
		return ResultPtr();
	}
	bool isFolder() const {
		return type == FRX_BROWSER_FOLDER;
	}
};
inline std::ostream & operator <<(std::ostream &os, const BrowserNode &n) {
	os<<n.name;
	return os;
}
//=============================================================================
// @class BrowserCellRenderer
//=============================================================================
template <class T>
struct FrxBrowserCellRenderer :
	public FrxParameterLabel
{
	//-------------------------------------------------------------------------
	typedef FrxParameterLabel Super;
	//-------------------------------------------------------------------------
	typedef FrxBrowserCellRenderer<T> Class;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<Class> Ptr;
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(Class)
	//-------------------------------------------------------------------------
	template <class ListType>
	sdc::AComponentPtr getListCellRendererComponent(
			boost::shared_ptr<ListType> list, // the list
			const T &value, // value to display
			int index, // cell index
			bool isSelected, // is the cell selected
			bool cellHasFocus // does the cell have focus
	);
	//-------------------------------------------------------------------------
	virtual ~FrxBrowserCellRenderer() {
	}
	//-------------------------------------------------------------------------
	virtual void installLookAndFeel(sdc::ui::ALookAndFeelPtr laf) {
		Super::installLookAndFeel(laf);
	}
};
///////////////////////////////////////////////////////////////////////////////
// ListCellRenderer impl.
//-----------------------------------------------------------------------------
template <class T>
template <class ListType>
sdc::AComponentPtr FrxBrowserCellRenderer<T>::getListCellRendererComponent(
	boost::shared_ptr<ListType> list, // the list
	const T &value, // value to display
	int index, // cell index
	bool isSelected, // is the cell selected
	bool cellHasFocus // does the cell have focus
)
{

	if (isSelected) {
		setBackground(list->getSelectionBackground());
		setForeground(list->getSelectionForeground());
	} else {
		setBackground(list->getBackground());
		setForeground(list->getForeground());
	}
	
	setText(sambag::com::toString(value.data));
	sd::ISurface::Ptr icon = 
		BrowserConstants::getIcon(value.data.type);
	if (icon)
		setIcon(icon);
		
	setEnabled(list->isEnabled());
	setFont(list->getFont());
	setValue(0.0);

	// specific callback
	if (value.data.drawCallback) {
		value.data.drawCallback(getPtr());
	}
	return getPtr();
}
//=============================================================================
/** 
  * @class FrxColumnBrowser.
  */
class FrxColumnBrowser : public sdc::FramedWindow {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef BrowserNode T;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxColumnBrowser> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxColumnBrowser> WPtr;
	//-------------------------------------------------------------------------
	sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const;
	//-------------------------------------------------------------------------
	typedef sdc::AColumnBrowser<
		sdc::DefaultTreeModel<T>,
		FrxBrowserCellRenderer
	> BrowserImpl;
	//-------------------------------------------------------------------------
	typedef sdc::FramedWindow Super;
protected:
	//-------------------------------------------------------------------------
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	FrxColumnBrowser( sdc::Window::Ptr parent=sdc::Window::Ptr() ) :
		sdc::FramedWindow(parent) {}
	//-------------------------------------------------------------------------
	virtual void installListeners() {}
	//-------------------------------------------------------------------------
	virtual void createMainBtns() {}
	//-------------------------------------------------------------------------
	IFrxColumnBrowserCtrl::Ptr ctrl;
private:
	//-------------------------------------------------------------------------
	sdc::AContainerPtr buttonPane;
	//-------------------------------------------------------------------------
	BrowserImpl::Ptr browser;
public:
	//-------------------------------------------------------------------------
	Ptr getPtr() {
		return boost::shared_dynamic_cast<FrxColumnBrowser>(AComponent::getPtr());
	}
	//-------------------------------------------------------------------------
	sdc::AContainerPtr getButtonPane() const {
		return buttonPane;
	}
	//-------------------------------------------------------------------------
	BrowserImpl::Ptr getBrowserImpl() const {
		return browser;
	}
	//-------------------------------------------------------------------------
	virtual void setCtrl(IFrxColumnBrowserCtrl::Ptr ctrl);
	//-------------------------------------------------------------------------
	virtual IFrxColumnBrowserCtrl::Ptr getCtrl() const;
	//-------------------------------------------------------------------------
	virtual void initTree(FrxCircuidViewPtr view);
}; // FrxColumnBrowser
}}} // namespace(s)

#endif /* SAMBAG_FRXCOLUMNBROWSER_H */
