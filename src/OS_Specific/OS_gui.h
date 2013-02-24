/*
 * ============================================================================
 * OS_com.h
 *      Author: Johannes Unger
 * ============================================================================
 */

#ifndef OS_GUI_H
#define OS_GUI_H

#include <sambag/disco/components/Window.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {

/**
 * if host won't resize main editor nestedWindow, we need this (ugly) fix.
 */
extern void osHostWontResizeFix(sdc::Window::Ptr, int width, int height);

}} //namespace
#endif 
