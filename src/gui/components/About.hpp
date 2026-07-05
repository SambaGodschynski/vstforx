/*
 * About.hpp
 *
 *  Created on: Thu Jul 11 14:51:19 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_ABOUT_H
#define SAMBAG_ABOUT_H

#include <memory>
#include <sambag/disco/components/FramedWindow.hpp>
#include <gui/HandyNamespaces.hpp>
#include <string>
#include <list>
#include <boost/timer/timer.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class About.
  */
typedef sdc::Window AboutBoxWindow;
class About : public AboutBoxWindow {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef AboutBoxWindow Super;
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<About> Ptr;
	//-------------------------------------------------------------------------
	typedef std::weak_ptr<About> WPtr;
protected:
	//-------------------------------------------------------------------------
	About( sdc::Window::Ptr parent=sdc::Window::Ptr() ) : Super(parent) {}
	//-------------------------------------------------------------------------
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	virtual void initView();
	//-------------------------------------------------------------------------
	typedef std::list<std::string> Lines;
	virtual void getTextLines( Lines &out );
	//-------------------------------------------------------------------------
	void onMouseEvent(void *src, const sdce::MouseEvent &ev);
private:
	//-------------------------------------------------------------------------
	typedef boost::timer::cpu_timer Clock;
	Clock clock;
public:
	//-------------------------------------------------------------------------
	const Clock & getClock() const { return clock; }
	//-------------------------------------------------------------------------
	SAMBAG_STD_WINDOW_CREATOR(About)
}; // About
}}} // namespace(s)

#endif /* SAMBAG_ABOUT_H */
