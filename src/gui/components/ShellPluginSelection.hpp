/*
 * ShellPluginSelection.hpp
 *
 *  Created on: Wed Dec  5 12:00:05 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_SHELLPLUGINSELECTION_H
#define SAMBAG_SHELLPLUGINSELECTION_H

#include <memory>
#include <sambag/disco/components/FramedWindow.hpp>
#include <sambag/disco/components/List.hpp>
#include <sambag/disco/components/events/ActionEvent.hpp>
#include <sambag/com/events/Events.hpp>
#include <processing/pluginTypes/VstShellPlugin.hpp>
#include <vector>
#include <processing/PlugInfo.h>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class ShellPluginSelection.
  * Shows ShellPlugin content to select.
  */
class ShellPluginSelection : 
	public sambag::disco::components::FramedWindow, 
	public sambag::com::events::EventSender<
		sambag::disco::components::events::ActionEvent
	>
{
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<ShellPluginSelection> Ptr;
	//-------------------------------------------------------------------------
	typedef sambag::disco::components::Window Window;
	//-------------------------------------------------------------------------
	typedef sambag::disco::components::FramedWindow Super;
	//-------------------------------------------------------------------------
	typedef sambag::disco::components::StringList List;
	//-------------------------------------------------------------------------
	typedef sambag::disco::components::OnCloseEvent OnCloseEvent;
	//-------------------------------------------------------------------------
	typedef sambag::disco::components::events::ActionEvent ActionEvent;
	//-------------------------------------------------------------------------
	typedef int VstPlugId;
protected:
	//-------------------------------------------------------------------------
	ShellPluginSelection(Window::Ptr parent = Window::Ptr()) : Super(parent) {}
	//-------------------------------------------------------------------------
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	List::Ptr list;
	//-------------------------------------------------------------------------
	void onBtnOk(void *, const ActionEvent &ev); 
	//-------------------------------------------------------------------------
	void onBtnCancel(void *, const ActionEvent &ev); 
	//-------------------------------------------------------------------------
	void createContent(); 
	//-------------------------------------------------------------------------
	void onWindowClose(void *, const OnCloseEvent &ev);
private:
	//-------------------------------------------------------------------------
	std::vector<std::string> plugids;
	//-------------------------------------------------------------------------
	::processing::PluginInfo plugInf;
public:
	//-------------------------------------------------------------------------
	void setShellPlugin(const ::processing::PluginInfo &pI);
	//-------------------------------------------------------------------------
	SAMBAG_STD_WINDOW_CREATOR(ShellPluginSelection)
	//-------------------------------------------------------------------------
	const ::processing::PluginInfo & getCurrentSelection();
	//-------------------------------------------------------------------------
	void addShellInfo(const ::processing::ShellPluginInfo &inf);
}; // ShellPluginSelection
}}} // namespace(s)

#endif /* SAMBAG_SHELLPLUGINSELECTION_H */
