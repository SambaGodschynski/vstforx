/*
 * FrxProcessorBrowser.hpp
 *
 *  Created on: Tue Oct 23 10:09:40 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPROCESSORBROWSER_H
#define SAMBAG_FRXPROCESSORBROWSER_H

#include <memory>
#include "FrxMainBrowser.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxProcessorBrowser.
  */
class FrxProcessorBrowser : public FrxMainBrowser {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<FrxProcessorBrowser> Ptr;
	//-------------------------------------------------------------------------
	typedef std::weak_ptr<FrxProcessorBrowser> WPtr;
	//-------------------------------------------------------------------------
	typedef FrxMainBrowser Super;
protected:
	//-------------------------------------------------------------------------
	FrxProcessorBrowser(sdc::Window::Ptr parent=sdc::Window::Ptr()) :
		 FrxMainBrowser(parent) {}
	//-------------------------------------------------------------------------
	virtual void postConstructor();
public:
	//-------------------------------------------------------------------------
	static Ptr create( sdc::Window::Ptr parent=sdc::Window::Ptr() );
private:
public:
}; // FrxProcessorBrowser
}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSORBROWSER_H */
