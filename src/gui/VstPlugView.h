/*
 * ===========================================================================================================
 * VstPlugView.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef VSTPLUGVIEW_H
#define VSTPLUGVIEW_H

#include "processing/pluginTypes/VSTPlugin2x.h" 
#include "processing/parameter/Parameter.h"
#include "OS_Specific/OS_processing.h"
#include "OS_Specific/OS_gui.h"
#include <map>
#include "vstgui.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "IdleListener.h"
#include "ViewEvents.h"
#include "com/one4All.h"

namespace ppiGui{
class GVSTPlugNode;
class PpiEditor;
//============================================================================================================
//	Klasse VSTPlugView:
//  Oeffnet ein neues Fenster mit AEffGUIEditor. 
//============================================================================================================
class VSTPlugView : 
	public EventSender<OnOpen>,
	public EventSender<OnClose>,
	public EventListener<OnOpen>,
	public EventListener<OnClose>,
	public EventListener<OnIdle>,
	public EventListener<OnMoving>,
	public EventSender<OnMoving>,
	public EventListener<processing::ResizeEditorEvent>
{
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<VSTPlugView> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	Window::Ptr win;
	//--------------------------------------------------------------------------------------------------------
	Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	PpiEditor *editor;
	//--------------------------------------------------------------------------------------------------------
	AEffect *aEff;
	//--------------------------------------------------------------------------------------------------------
	VSTPlugView() {}
	//--------------------------------------------------------------------------------------------------------
	VSTPlugView ( PpiEditor *ed, processing::VSTPlugin::Ptr vstPlugNode );
	//--------------------------------------------------------------------------------------------------------
	processing::VSTPlugin::Ptr vstPlugNode;
protected:
	//--------------------------------------------------------------------------------------------------------
	boost::weak_ptr<VSTPlugView> self;
public:
	//--------------------------------------------------------------------------------------------------------
	bool isOpen() const { 
		if (!win) return false;
		return win->isOpen();
	}
	//--------------------------------------------------------------------------------------------------------
	static Ptr create ( PpiEditor *ed, processing::VSTPlugin::Ptr vstPlugNode ); 
	//--------------------------------------------------------------------------------------------------------
	Ptr getPtr() { return self.lock(); }
	//--------------------------------------------------------------------------------------------------------
	Window::Ptr getWindow() const { return win; }
	//--------------------------------------------------------------------------------------------------------
	~VSTPlugView();
	//--------------------------------------------------------------------------------------------------------
	void openWindow( size_t posX = 0, size_t posY = 0 );
	//--------------------------------------------------------------------------------------------------------
	void closeWindow();
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler( void *scr, const OnIdle &ev );
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler( void *scr, const OnOpen &ev );
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler( void *scr, const OnClose &ev );
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *src, const OnMoving &ev );
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler( void *scr, const processing::ResizeEditorEvent &ev );
};
}

#endif


