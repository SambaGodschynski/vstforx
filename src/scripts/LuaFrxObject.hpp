/*
 * LuaFrxObject.hpp
 *
 *  Created on: Fri Mar 14 11:47:35 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_LUAFRXOBJECT_H
#define SAMBAG_LUAFRXOBJECT_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/lua/ALuaObject.hpp>
#include <processing/ModelObject.hpp>
#include <gui/ViewObject.hpp>
#include <gui/IViewModelMap.hpp>
#include <boost/unordered_map.hpp>
#include <gui/components/Forward.hpp>
#include <map>
#include <boost/function.hpp>
#include <loki/Singleton.h>
#include <sambag/disco/components/PopupMenu.hpp>
#include <sambag/disco/components/Label.hpp>

namespace frx { namespace scripts {
namespace slua = sambag::lua;
//=============================================================================
/** 
  * @class LuaFrxObject.
  * @brief lua representation of a @see FrxComponent.
  * @because the view object is only available for one editor session
  * we save the model object and access the view object via the view2model map.
  */
class LuaFrxObject : public slua::ALuaObject {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef slua::ALuaObject Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<LuaFrxObject> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<LuaFrxObject> WPtr;
    //-------------------------------------------------------------------------
    typedef frx::gui::IViewModelMap ViewModelMap;
    //-------------------------------------------------------------------------
    typedef boost::weak_ptr<ViewModelMap> ViewModelMapWPtr;
    //-------------------------------------------------------------------------
    typedef frx::processing::ModelObject ModelObject;
    //-------------------------------------------------------------------------
    typedef frx::gui::ViewObject ViewObject;
    //-------------------------------------------------------------------------
    typedef boost::unordered_map<UId, LuaFrxObject::WPtr> UIdMap;
    //-------------------------------------------------------------------------
    struct Factory {
        friend struct Loki::CreateUsingNew<Factory>;
        typedef boost::function<LuaFrxObject::Ptr(lua_State * lua,
            ModelObject::Ptr obj, ViewModelMap::Ptr map)> Creator;
        typedef std::map<std::string, Creator> CreatorMap;
        //---------------------------------------------------------------------
        /**
        * @brief registeres creator function. Same purpose as @see ViewFactory and
        * @see ModelFactory
        * @note id structure is mostly the same as in @see ViewFactory and
        * @see ModelFactory: e.g.: frx.lua.internal.ADSTrigger
        * frx.lua.parameter.Parameter, frx.lua.connections.ParameterConnection ...
        */
        bool registerCreator(const std::string &id, const Creator &f);
        //---------------------------------------------------------------------
        /**
        * @brief creates object by creator id and pushes to lua stack
        * @see ModelFactory
        * @param creator id. @see registerCreator()
        */
        LuaFrxObject::Ptr createAndPush(const std::string &id,
            lua_State * lua, ModelObject::Ptr obj, ViewModelMap::Ptr map);
        //---------------------------------------------------------------------
        /**
        * @return true if id is registered as creator
        */
        bool isRegistered(const std::string &id);
        //---------------------------------------------------------------------
        static Factory & instance();
        private:
            CreatorMap creatorMap;
    };
private:
    //-------------------------------------------------------------------------
    ViewModelMapWPtr modelMap; // modelObject2ViewObject
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
    /**
     * @return related view object.
     * @note pushes lua error if not available
     */
    virtual frx::gui::components::FrxComponentPtr
    getViewObject(lua_State *lua) const;
    //-------------------------------------------------------------------------
    LuaFrxObject();
    //-------------------------------------------------------------------------
    void setModelObject(ModelObject::Ptr obj);
    //-------------------------------------------------------------------------
    void setViewModelMap(ViewModelMap::Ptr map);
    //-------------------------------------------------------------------------
    /**
     * @brief called when lua object will be removed.
     */
    virtual void __lua_gc(lua_State *lua);
    //-------------------------------------------------------------------------
    typedef boost::tuple<float,float> Point;
    SAMBAG_LUA_FTAG(getLocation, Point());
    SAMBAG_LUA_FTAG(setLocation, void(float, float));
    SAMBAG_LUA_FTAG(getSize, Point());
    SAMBAG_LUA_FTAG(setSize, void(float, float));
    SAMBAG_LUA_FTAG(setName, void(std::string));
    SAMBAG_LUA_FTAG(getName, std::string());
    SAMBAG_LUA_FTAG(getTypeId, std::string());
    SAMBAG_LUA_FTAG(setMenu, void());
    typedef LOKI_TYPELIST_8(Frx_getLocation_Tag,
        Frx_setLocation_Tag,
        Frx_getSize_Tag,
        Frx_setSize_Tag,
        Frx_setName_Tag,
        Frx_getName_Tag,
        Frx_getTypeId_Tag,
        Frx_setMenu_Tag
    ) Functions;
    //////////////////////////////////////////////////////////////////////////fr/
    // lua2frx impl
    virtual std::string toString(lua_State *lua) const;
    virtual bool isequal(lua_State *lua) const;
    boost::tuple<float,float> getLocation(lua_State *lua);
    void setLocation(lua_State *lua, float x, float y);
    boost::tuple<float,float> getSize(lua_State *lua);
    void setSize(lua_State *lua, float x, float y);
    void setName(lua_State *lua, const std::string &name);
    std::string getName(lua_State *lua) const;
    std::string getTypeId(lua_State *lua) const;
    void addMenuEntry(sambag::disco::components::PopupMenuPtr res, lua_State *lua, int index);
    void setMenu(lua_State *lua);
public:
    //-------------------------------------------------------------------------
    const std::string & getTypeId() const {
        return typeId;
    }
    //-------------------------------------------------------------------------
    /**
     * @throws IllegalStateException
     */
    ViewModelMap::Ptr getViewModelMap() const;
    //-------------------------------------------------------------------------
    static LuaFrxObject::Ptr getByUId(const UId &uid);
    //-------------------------------------------------------------------------
    /**
     * @brief gets lua object by lua state stack
     * assumes that -1 in stack is a LuaFrxObject(table).
     * @return a LuaFrxObject pointer
     * @throws IllegalStateException
     */
    static LuaFrxObject::Ptr getFromLuaStack(lua_State *lua, int index = -1);
    //-------------------------------------------------------------------------
    /**
     * @return related model object.
     * @throws IllegalStateException if not available
     */
    virtual ModelObject::Ptr getModelObject() const;
    //-------------------------------------------------------------------------
    /**
     * @return related view object.
     * @throws IllegalStateException if not available
     */
    virtual frx::gui::components::FrxComponentPtr getViewObject() const;
    //-------------------------------------------------------------------------
    virtual void addLuaFields(lua_State *lua, int index);
    //-------------------------------------------------------------------------
    virtual ~LuaFrxObject() {}
private:
public:
}; // LuaFrxObject
}} // namespace(s)

#endif /* SAMBAG_LUAFRXOBJECT_H */
