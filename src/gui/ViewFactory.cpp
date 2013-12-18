/*
 * ViewFactory.cpp
 *
 *  Created on: Wed Dec 18 10:05:20 2013
 *      Author: Johannes Unger
 */

#include "ViewFactory.hpp"


namespace frx { namespace processing { 
typedef Loki::SingletonHolder<ViewFactory> ViewFactoryHolder;
//=============================================================================
//  Class ViewFactory
//=============================================================================
//-----------------------------------------------------------------------------
ViewFactory & ViewFactory::instance() {
	return ViewFactoryHolder::Instance();
}
}} // namespace(s)
