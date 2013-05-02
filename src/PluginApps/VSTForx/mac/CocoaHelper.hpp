/*
 * CocoaHelper.hpp
 *
 *  Created on: Thu Apr 25 13:33:51 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_COCOAHELPER_H
#define SAMBAG_COCOAHELPER_H

#include <string>

namespace frx { namespace com { 
//=============================================================================
/** 
  * @class CocoaHelper.
  */
class CocoaHelper {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	static std::string getBundleLocation();
	//-------------------------------------------------------------------------
	/**
	 * @return full path of resource file
	 */
	static std::string getResourceLocation(const std::string &path);
}; // CocoaHelper
}} // namespace(s)

#endif /* SAMBAG_COCOAHELPER_H */
