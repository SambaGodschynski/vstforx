/*
 * SetupWindow.hpp
 *
 *  Created on: Thu Oct 25 10:16:26 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_SETUPWINDOW_H
#define SAMBAG_SETUPWINDOW_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/FramedWindow.hpp>
#include <sambag/disco/components/Forward.hpp>
#include <sambag/disco/components/List.hpp>
#include <sambag/disco/components/events/ActionEvent.hpp>
#include <sambag/disco/components/Forward.hpp>
#include <sambag/disco/components/CheckBox.hpp>
#include "SetupCtrl.hpp"
#include "Forward.hpp"
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class SetupWindow.
  */
class SetupWindow : public sdc::FramedWindow {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<SetupWindow> Ptr;
	//-------------------------------------------------------------------------
	typedef sdc::FramedWindow Super;
protected:
	//-------------------------------------------------------------------------
	SetupWindow( sdc::Window::Ptr parent=sdc::Window::Ptr() ) :
		sdc::FramedWindow(parent) {}
	//-------------------------------------------------------------------------
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	virtual sdc::AContainerPtr createSetupPane();
	//-------------------------------------------------------------------------
	virtual sdc::AContainerPtr createDirListPane();
	//-------------------------------------------------------------------------
	virtual sdc::AContainerPtr createDirListBtnPane();
	//-------------------------------------------------------------------------
	virtual sdc::AContainerPtr createMainBtnPane();
	//-------------------------------------------------------------------------
	virtual sdc::StringList::Ptr createDirList();
	//-------------------------------------------------------------------------
	virtual sdc::ScrollPanePtr createDirListScrollPane();
	//-------------------------------------------------------------------------
	sdc::AContainerPtr createMiscPane();
	//-------------------------------------------------------------------------
	sdc::AContainerPtr createWindowSizePane();
private:
	//-------------------------------------------------------------------------
	struct ResizeBtnHandler;
	//-------------------------------------------------------------------------
	boost::shared_ptr<ResizeBtnHandler> rszBtnHandler;
	//-------------------------------------------------------------------------
	SetupCtrl::Ptr ctrl;
	//-------------------------------------------------------------------------
	sdc::AContainerPtr dirListPane, dirListBtnPane, mainBtnPane;
	//-------------------------------------------------------------------------
	sdc::StringList::Ptr dirList;
	//-------------------------------------------------------------------------
	sdc::ScrollPanePtr dirListScrollPane;
	//-------------------------------------------------------------------------
	sdc::CheckBox::Ptr chkbxFS;
	//-------------------------------------------------------------------------
	typedef void (SetupWindow::*BtnActionFunc)
		(void *, const sdc::events::ActionEvent&);
	//-------------------------------------------------------------------------
	sdc::ButtonPtr createBtn(BtnActionFunc f, const std::string &txt);
	//-------------------------------------------------------------------------
	void updateSettings();
	//-------------------------------------------------------------------------
	void saveSettings();
	//-------------------------------------------------------------------------
	void cancelSettings();
	//-------------------------------------------------------------------------
	void openScanningDialog();
public:
	//-------------------------------------------------------------------------
	virtual ~SetupWindow();
	//-------------------------------------------------------------------------
	void setCtrl(SetupCtrl::Ptr ctrl);
	//-------------------------------------------------------------------------
	SetupCtrl::Ptr getCtrl() const;
	//-------------------------------------------------------------------------
	void onFastScanSelected(void *, const sdc::events::ActionEvent &ev);
	//-------------------------------------------------------------------------
	void onBtnOkPressed(void *, const sdc::events::ActionEvent &ev);
	//-------------------------------------------------------------------------
	void onBtnCancelPressed(void *, const sdc::events::ActionEvent &ev);
	//-------------------------------------------------------------------------
	void onBtnAddDirPressed(void *, const sdc::events::ActionEvent &ev);
	//-------------------------------------------------------------------------
	void onBtnChangeDirPressed(void *, const sdc::events::ActionEvent &ev);
	//-------------------------------------------------------------------------
	void onBtnRemoveDirPressed(void *, const sdc::events::ActionEvent &ev);
	//-------------------------------------------------------------------------
	sdc::AContainerPtr getDirListPane() const {
		return dirListPane;
	}
	//-------------------------------------------------------------------------
	sdc::AContainerPtr getDirListBtnPane() const {
		return dirListBtnPane;
	}
	//-------------------------------------------------------------------------
	sdc::AContainerPtr getMainBtnPane() const  {
		return mainBtnPane;
	}
	//-------------------------------------------------------------------------
	sdc::StringList::Ptr getDirList() const  {
		return dirList;
	}
	//-------------------------------------------------------------------------
	sdc::ScrollPanePtr getDirListScrollPane() const  {
		return dirListScrollPane;
	}
	//-------------------------------------------------------------------------
	SAMBAG_STD_WINDOW_CREATOR(SetupWindow)
}; // SetupWindow
}}} // namespace(s)
#endif /* SAMBAG_SETUPWINDOW_H */
