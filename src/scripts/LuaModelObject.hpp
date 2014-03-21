/*
 * LuaModelObject.hpp
 *
 *  Created on: Fri Mar 14 11:47:35 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_LuaModelObject_H
#define SAMBAG_LuaModelObject_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/lua/ALuaObject.hpp>
#include <processing/ModelObject.hpp>
#include <gui/IViewModelMap.hpp>
#include <boost/unordered_map.hpp>
#include <map>
#include <boost/function.hpp>

namespace frx { namespace scripts {
namespace slua = sambag::lua;
//=============================================================================
/** 
  * @class LuaModelObject.
  */
class LuaModelObject : public slua::ALuaObject {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef slua::ALuaObject Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaModelObject> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<LuaModelObject> WPtr;
    //-------------------------------------------------------------------------
    typedef frx::processing::ModelObject ModelObject;
    //-------------------------------------------------------------------------
    typedef boost::unordered_map<UId, LuaModelObject::WPtr> UIdMap;
private:
    //-------------------------------------------------------------------------
    ModelObject::WPtr obj;
    //-------------------------------------------------------------------------
    static UIdMap uidMap;
    //-------------------------------------------------------------------------
    std::string typeId;
protected:
    //-------------------------------------------------------------------------
    void setTypeId(const std::string &typeId);
    //-------------------------------------------------------------------------
    LuaModelObject();
    //-------------------------------------------------------------------------
    void setModelObject(ModelObject::Ptr obj);
    //-------------------------------------------------------------------------
    /**
     * @brief called when lua object will be removed.
     */
    virtual void __gc(lua_State *lua);
    ///////////////////////////////////////////////////////////////////////////
    // lua2frx impl
    virtual bool isequal(lua_State *lua) const;
public:
    //-------------------------------------------------------------------------
    std::string getTypeId() const {
        return typeId;
    }
    //-------------------------------------------------------------------------
    static LuaModelObject::Ptr getByUId(const UId &uid);
    //-------------------------------------------------------------------------
    /**
     * @brief gets lua object by lua state stack
     * assumes that -1 in stack is a LuaModelObject(table).
     * @return a LuaModelObject pointer
     * @throws IllegalStateException
     */
    static LuaModelObject::Ptr getFromLuaStack(lua_State *lua, int index = -1);
    //-------------------------------------------------------------------------
    /**
     * @return related model object.
     * @throws IllegalStateException if not available
     */
    virtual ModelObject::Ptr getModelObject() const;
    //-------------------------------------------------------------------------
    virtual void addLuaFields(lua_State *lua, int index);
    //-------------------------------------------------------------------------
    virtual ~LuaModelObject() {}
private:
public:
}; // LuaModelObject
}} // namespace(s)

#endif /* SAMBAG_LuaModelObject_H */
