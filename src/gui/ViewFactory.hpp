/*
 * ViewFactory.hpp
 *
 *  Created on: Wed Dec 18 10:05:20 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_VIEWFACTORY_H
#define SAMBAG_VIEWFACTORY_H

#include <loki/Singleton.h>
namespace frx { namespace processing { 
//=============================================================================
/** 
  * @class ViewFactory.
  */
class ViewFactory {
//=============================================================================
friend struct Loki::CreateUsingNew<ViewFactory>;
public:
protected:
private:
public:
	//-------------------------------------------------------------------------
	static ViewFactory & instance();
    
}; // ViewFactory
}} // namespace(s)

#endif /* SAMBAG_VIEWFACTORY_H */
