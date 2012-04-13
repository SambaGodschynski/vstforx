/*
 * ===========================================================================================================
 * ScriptInfo.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_SCRIPTINFO_H
#define FORX_SCRIPTINFO_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/Serialization.h"
#include <sambag/lua/LuaMap.hpp>
#include <string>

namespace com {
//============================================================================================================
/**
 * @class ScriptInfo.
 * POD-Kontainer
 */
//============================================================================================================
struct ScriptInfo {
	// processor setup
	std::string name;
	std::string location;
	ScriptInfo(const std::string &location = "") :
		location(location)
	{
	}
};
}// namespace com

#endif  // FORX_SCRIPTINFO_H


