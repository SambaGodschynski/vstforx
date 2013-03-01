/*
 * hostWontResizeFix.cpp
 *
 *  Created on: Sun Feb  24 12:58:07 2013
 *      Author: Johannes Unger
 */
#include "OS_Specific/OS_gui.h"

#ifdef FRX_OS_WINDOWS

#include <windows.h>

namespace frx { namespace gui {
//-----------------------------------------------------------------------------
/**
 * adapted from vstgui.
 */
void osHostWontResizeFix(sdc::Window::Ptr win, int width, int height) {
	if (!win) {
		return;
	}
	RECT  rctTempWnd, rctParentWnd;
	HWND  hTempWnd;
	long  iFrame = (2 * GetSystemMetrics (SM_CYFIXEDFRAME));
	
	long diffWidth  = 0;
	long diffHeight = 0;
	
	hTempWnd = (HWND)win->getWindowImpl()->getSystemHandle();
	if (!hTempWnd) {
		return;
	}
	while ((diffWidth != iFrame) && (hTempWnd != NULL)) // look for FrameWindow
	{
		HWND hTempParentWnd = GetParent (hTempWnd);
		TCHAR buffer[1024];
		GetClassName (hTempParentWnd, buffer, 1024);
		if (!hTempParentWnd || !strcmp (buffer, TEXT("MDIClient")))
			break;
		GetWindowRect (hTempWnd, &rctTempWnd);
		GetWindowRect (hTempParentWnd, &rctParentWnd);
		
		SetWindowPos (hTempWnd, HWND_TOP, 0, 0, width + diffWidth, height + diffHeight, SWP_NOMOVE);
		
		diffWidth  += (rctParentWnd.right - rctParentWnd.left) - (rctTempWnd.right - rctTempWnd.left);
		diffHeight += (rctParentWnd.bottom - rctParentWnd.top) - (rctTempWnd.bottom - rctTempWnd.top);
		
		if ((diffWidth > 80) || (diffHeight > 80)) // parent belongs to host
			return;

		if (diffWidth < 0)
			diffWidth = 0;
        if (diffHeight < 0)
			diffHeight = 0;
		
		hTempWnd = hTempParentWnd;
	}
	
	if (hTempWnd) {
		SetWindowPos (hTempWnd, HWND_TOP, 0, 0, width + diffWidth, height + diffHeight, SWP_NOMOVE);
	}
}

}} // namespace(s)

#endif // FRX_OS_WINDOWS
