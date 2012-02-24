/*
 * ===========================================================================================================
 * Resources.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef RESOURCES_H
#define RESOURCES_H

#include "vstgui.h"
#include "boost/shared_ptr.hpp"

namespace ppiGui {
extern const long CHILDW_ICON;
extern const long CONTEXT_CURSOR;
//============================================================================================================
class Resources {
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<Resources> Ptr;
	//--------------------------------------------------------------------------------------------------------
	enum BitmapID { // !! um abwaerts komp. zu bleiben(beim restore): nur am ende einfuegen !!
		VSTPLUG_NODE,
		VSTPLUG_INPUT,
		VSTPLUG_OUTPUT,
		VSTPLUG_OUTPUT_GLOW,
		MAIN_INPUT_NODE,
		MAIN_OUTPUT_NODE,
		STD_KNOB,
		PASSIVE_KNOB,
		VOLUME_ADAPTER,
		STEP_ADAPTER,
		OSWITCH_ADAPTER,
		ADSR_ADAPTER,
		PEAK_TRACK_ADAPTER,
		CHANNEL_SPLIT_ADAPTER,
		EDIT_BUTTON,
	    MENU_ROCKER,
	    TLBX_BK,
	    TLBX_BTN_USE,
	    TLBX_BTN_CNT,
	    TLBX_BTN_MOVE,
		TLBX_BTN_SETUP,
		DLG_SYSM_EXIT,
		DLG_SETTINGS_FRAME,
		DLG_BTN_REMOVE_DIR,
		DLG_BTN_ADD_DIR,
		DLG_BTN_CANCEL,
		DLG_BTN_CHDIR,
		DLG_BTN_OK,
		DLG_CHKBX_FASTSCAN,
		DLG_SCANNING_FRAME,
		DLG_RESIZE_ROCKER,
		DLG_RESIZE_ROCKER_WIDTH,
		DLG_RESIZE_ROCKER_HEIGHT,
		DLG_SCAN_NOW,
		MIDI_RECEIVER,
		BACKGROUND,
		ADSR_SWITCH,
		NUM_BITMAPS
	};	
	//--------------------------------------------------------------------------------------------------------
	// TODO: Ideal waehre eine Wrapper Klasse fuer CBitmap die nicht das orginal zerstoert wie 
	// zb.: CBitmap::forget();
	CBitmap * getResourceBitmap( BitmapID id ) { 
		return bitmaps[id]; 
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~Resources();
	//--------------------------------------------------------------------------------------------------------
	// Zum CBitmap Init. muss editor schon vorhanden sein. Sonst absturz ( unter OSX ) 
	static Ptr initResources( AEffGUIEditor *editor );
private:
	//--------------------------------------------------------------------------------------------------------
	Resources();
	//--------------------------------------------------------------------------------------------------------
	CBitmap **bitmaps;
	//--------------------------------------------------------------------------------------------------------
	static Ptr singleton;
}; // class Resources
extern Resources::Ptr resources;
} // ppiGui

#endif