/*
 * LuaDefs.hpp
 *
 *  Created on: Wen Feb  7 13:30:58 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_LUADEFS_H
#define SAMBAG_LUADEFS_H

#include <tuple>
#include <sambag/lua/Lua.hpp>
#include <sambag/lua/LuaMap.hpp>
#include <sambag/lua/LuaHelper.hpp>
#include <sambag/lua/LuaSequence.hpp>
#include <boost/tuple/tuple.hpp>
#include <processing/Frames.h>

namespace frx { namespace processing {
    typedef sambag::lua::LuaSequenceEx< ::processing::Frames::T > LuaFloatSeqEx;
    typedef sambag::lua::LuaSequence< ::processing::Frames::T > LuaFloatSeq;
    typedef std::tuple< LuaFloatSeqEx > LuaFrames;
    typedef std::tuple< LuaFloatSeq, LuaFloatSeq > FFTData;
}}

#endif // SAMBAG_LUADEFS_H
