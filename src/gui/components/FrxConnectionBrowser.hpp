/*
 * FrxConnectionBrowser.hpp
 *
 *  Created on: Fri Dec  7 14:33:22 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCONNECTIONBROWSER_H
#define SAMBAG_FRXCONNECTIONBROWSER_H

#include <boost/shared_ptr.hpp>
#include "FrxMainBrowser.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxConnectionBrowser.
  */
class FrxConnectionBrowser : public FrxMainBrowser {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxConnectionBrowser> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxConnectionBrowser> WPtr;
	//-------------------------------------------------------------------------
	typedef FrxMainBrowser Super;
protected:
	//-------------------------------------------------------------------------
	FrxConnectionBrowser(sdc::Window::Ptr parent=sdc::Window::Ptr()) :
		 FrxMainBrowser(parent) {}
	//-------------------------------------------------------------------------
	virtual void postConstructor();
public:
	//-------------------------------------------------------------------------
	static Ptr create( sdc::Window::Ptr parent=sdc::Window::Ptr() );
private:
public:
}; // FrxConnectionBrowser
}}} // namespace(s)

#endif /* SAMBAG_FRXCONNECTIONBROWSER_H */
