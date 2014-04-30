/*
 * LuaParameter.hpp
 *
 *  Created on: Fri Mar 14 11:47:45 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_LUAPARAMETER_H
#define SAMBAG_LUAPARAMETER_H

#include <boost/shared_ptr.hpp>
#include "LuaModelObject.hpp"

namespace frx { namespace scripts {

//=============================================================================
/** 
  * @class LuaParameter.
  * @brief lua representation of a @see Parameter
  */
class LuaParameter : public LuaModelObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaParameter> Ptr;
    //-------------------------------------------------------------------------
    typedef LuaModelObject Super;
protected:
    //-------------------------------------------------------------------------
    void setValue(float v);
    //-------------------------------------------------------------------------
    LuaParameter();
    float getValue() const;
    std::string getName() const;
    ///////////////////////////////////////////////////////////////////////////
    // lua impl.
    void setValue(lua_State * lua, float v);
    float getValue(lua_State * lua);
    std::string getName(lua_State * lua);
    std::string toString(lua_State * lua) const;
private:
public:
    //-------------------------------------------------------------------------
    static Ptr createAndPush(lua_State * lua, ModelObject::Ptr obj);
}; // LuaParameter
}} // namespace(s)

#endif /* SAMBAG_LUAPARAMETER_H */
