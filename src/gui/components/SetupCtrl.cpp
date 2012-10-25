/*
 * SetupCtrl.cpp
 *
 *  Created on: Thu Oct 25 13:56:01 2012
 *      Author: Johannes Unger
 */

#include "SetupCtrl.hpp"

namespace com {
extern std::string osSelectDirectory(const std::string &wndTitle, 
						    const std::string &startPath);
} // namespace com

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class SetupCtrl
//=============================================================================
//-----------------------------------------------------------------------------
std::string SetupCtrl::selectDirectory(const std::string &startDir) const {
	return ::com::osSelectDirectory("select directory", startDir);
}

}}} // namespace(s)
