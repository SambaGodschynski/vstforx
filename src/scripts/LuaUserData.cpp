/*
 * LuaUserData.cpp
 *
 *  Created on: Fri Aug 29 10:43:36 2014
 *      Author: Johannes Unger
 */

#include "LuaUserData.hpp"

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaUserData
//=============================================================================
//-----------------------------------------------------------------------------
void LuaUserData::add(const std::string &key, const std::string &value) {
    container.add(key+".data", value);
}
}} // namespace(s)
