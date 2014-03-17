/*
 * LuaParameter.hpp
 *
 *  Created on: Fri Mar 14 11:47:45 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_LUAPARAMETER_H
#define SAMBAG_LUAPARAMETER_H

#include <boost/shared_ptr.hpp>
#include "LuaFrxObject.hpp"

namespace frx { namespace scripts {

//=============================================================================
/** 
  * @class LuaParameter.
  * @brief lua representation of a @see Parameter
  */
class LuaParameter : public LuaFrxObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaParameter> Ptr;
    //-------------------------------------------------------------------------
    typedef LuaFrxObject Super;
protected:
    //-------------------------------------------------------------------------
    void setValue(float v);
    //-------------------------------------------------------------------------
    SAMBAG_LUA_FTAG(setValue, void(float));
    typedef LOKI_TYPELIST_1(Frx_setValue_Tag) Functions;
    //-------------------------------------------------------------------------
    virtual void addLuaFields(lua_State * lua, int index);
    //-------------------------------------------------------------------------
    LuaParameter();
private:
public:
    //-------------------------------------------------------------------------
    static Ptr create(lua_State * lua, const std::string &name);
}; // LuaParameter
}} // namespace(s)

#endif /* SAMBAG_LUAPARAMETER_H */
