/*
 * ============================================================================
 * LuaFrxParameterConnection.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */

#include "LuaFrxParameterConnection.hpp"
#include <gui/components/FrxConnection.hpp>
#include <sambag/com/Common.hpp>
#include <processing/ParameterConnection.hpp>
#include "LuaFrxParameter.hpp"

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaFrxParameterConnection
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxParameterConnection::__lua_gc(lua_State *lua) {
    slua::unregisterClassFunctions<Functions>(getUId());
    Super::__lua_gc(lua);
}
//-----------------------------------------------------------------------------
void LuaFrxParameterConnection::addOperator(lua_State *lua, const std::string &opName)
{
    try {
        processing::ParameterConnection::Ptr mObj =
            boost::dynamic_pointer_cast<processing::ParameterConnection>(getModelObject());
        if (!mObj) {
            throw std::logic_error("unexpected error while accessing model object.");
        }
        mObj->addParameterCnOp(opName);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unkown error");
    }

}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxParameterConnection::getParameters(lua_State *lua) {
    using frx::processing::IParameter;
    using namespace frx::gui;
    using namespace frx::gui::components;
    try {
        ModelObject::Ptr obj = getModelObject();
        IViewModelMap::Ptr map;
        map = getViewModelMap();
        ModelObject::Parameters parameters;
        obj->getParameters("*", parameters);
        lua_createtable(lua, parameters.size(), 0);
        int top = lua_gettop(lua);
        int lua_index = 0;
        BOOST_FOREACH(IParameter::Ptr x, parameters) {
            lua_pushinteger(lua, ++lua_index);
            LuaFrxParameter::createAndPush(lua, x, map, "frx.lua.parameter.StdKnob");
            int table = lua_gettop(lua);
            lua_pushstring(lua, getUId().c_str());
            lua_setfield(lua, table, "__related");
            lua_settable(lua, top);
        }
        return slua::IgnoreReturn();
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unkown error");
    }
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
slua::LuaSequence<std::string>
LuaFrxParameterConnection::getOperatorNames(lua_State *lua)
{
    slua::LuaSequence<std::string> res;
    try {
        processing::ParameterConnection::Ptr mObj =
            boost::dynamic_pointer_cast<processing::ParameterConnection>(getModelObject());
        if (!mObj) {
            throw std::logic_error("unexpected error while accessing model object.");
        }
        size_t num = mObj->getNumConnectionOps();
        res.reserve(num);
        for(size_t i=0; i<num; ++i) {
            res.push_back(mObj->getConnectionOpName(i));
        }
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unkown error");
    }
    return res;
}
//-----------------------------------------------------------------------------
void LuaFrxParameterConnection::removeOperatorAt(lua_State *lua, int index) {
    index-=1;
    try {
        processing::ParameterConnection::Ptr mObj =
            boost::dynamic_pointer_cast<processing::ParameterConnection>(getModelObject());
        if (!mObj) {
            throw std::logic_error("unexpected error while accessing model object.");
        }
        mObj->removeConnectionOp(index);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unkown error");
    }
}
//-----------------------------------------------------------------------------
void LuaFrxParameterConnection::addLuaFields(lua_State *lua, int index) {
    Super::addLuaFields(lua, index);
    // 1-10
    sambag::lua::registerClassFunctions<Functions1,
        sambag::lua::TupleAccessor>
    (
        lua,
        boost::make_tuple(
            boost::bind(&LuaFrxParameterConnection::getOperatorNames, this, lua),
            boost::bind(&LuaFrxParameterConnection::removeOperatorAt, this, lua, _1),
            boost::bind(&LuaFrxParameterConnection::addOperator, this, lua, _1),
            boost::bind(&LuaFrxParameterConnection::getParameters, this, lua)
        ),
        index,
        getUId()
    );

}
//-----------------------------------------------------------------------------
LuaFrxParameterConnection::Ptr
LuaFrxParameterConnection::createAndPush(lua_State *lua,
    ModelObject::Ptr obj, ViewModelMap::Ptr map, const std::string &typeId)
{
    Ptr res(new LuaFrxParameterConnection());
    res->setModelObject(obj);
    res->setViewModelMap(map);
    res->setTypeId(typeId);
    res->createLuaObject(lua, "lua_parameterconnection");
    return res;
}
}} // namespace(s)
