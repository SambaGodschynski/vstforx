/*
 * ============================================================================
 * LuaFrxView.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */

#include "LuaFrxView.hpp"
#include <sambag/com/Common.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <gui/components/VstForxEditor.hpp>
#include <gui/components/IFrxComponentFactory.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/FrxComponent.hpp>
#include <gui/components/FrxProcessorNode.hpp>
#include <gui/IFrxControl.hpp>
#include "LuaFrxProcessor.hpp"
#include "LuaFrxIO.hpp"
#include <com/one4All.h>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaFrxView
//=============================================================================
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::addProcessor(lua_State *lua, const std::string &id)
{
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = getView(lua);
    if (!view) {
        return slua::IgnoreReturn();
    }
	IFrxComponentFactory &fac = getComponentFactory(view);
	IFrxControl &frxctrl = getFrxControl(view);
    IViewModelMap::Ptr map = getViewModelMap(view);
	FrxProcessorNodePtr res;
    try {
        res = fac.getProcessorCreator(id)(view);
    } catch (...) {
    }
	if (!res) {
        slua::pushLuaError(lua, "creation of " + id + " failed.");
		return slua::IgnoreReturn();
	}
  	frxctrl.addProcessorToView(view, res);
    LuaFrxProcessor::createAndPush(lua, map->getModelObject(res), map);
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
void LuaFrxView::remove(lua_State *lua) {
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = getView(lua);
    if (!view) {
        return;
    }
    try {
        // get object
        LuaFrxObject::Ptr obj = LuaFrxObject::getFromLuaStack(lua);
        // remove object
        IFrxControl &frxctrl = getFrxControl(view);
        frxctrl.removeComponent(view, obj->getViewObject());
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, std::string("removing failed: ") + ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "removing failed: unkown error");
    }
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::connect(lua_State *lua) {
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = getView(lua);
    if (!view) {
        return slua::IgnoreReturn();
    }
    try {
        // get objects
        LuaFrxObject::Ptr a = LuaFrxObject::getFromLuaStack(lua, -1);
        LuaFrxObject::Ptr b = LuaFrxObject::getFromLuaStack(lua, -2);
        // remove object
        IFrxControl &frxctrl = getFrxControl(view);
        FrxNode::Ptr na = boost::dynamic_pointer_cast<FrxNode>(a->getViewObject());
        FrxNode::Ptr nb = boost::dynamic_pointer_cast<FrxNode>(b->getViewObject());
        if (!na || !nb) {
            return false;
        }
        return frxctrl.connect(view, na, nb);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, std::string("connecting failed: ") + ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "connecting failed: unkown error");
    }
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::getObjects(lua_State *lua) {
    using namespace frx::gui;
	using namespace frx::gui::components;
    using namespace sambag::disco::components;
    FrxCircuidViewPtr view = getView(lua);
    if (!view) {
        return slua::IgnoreReturn();
    }
    IViewModelMap::Ptr map = getViewModelMap(view);
    const FrxCircuidView::Components &comps =
        view->getContentPane()->getComponents();
	
    lua_createtable(lua, comps.size(), 0);
    int top = lua_gettop(lua);
    
    int lua_index = 1;
    BOOST_FOREACH(AComponentPtr c, comps) {
        FrxComponent::Ptr vObj = boost::dynamic_pointer_cast<FrxComponent>(c);
        if (!vObj) {
            continue;
        }
        processing::ModelObject::Ptr mObj = map->getModelObject(vObj);
        std::string type;
        vObj->getClientProperty("frx.component.type", type); // set in FrxComponentFactory
                                                             // or in FrxConcreteXXX postConstructor
        if (type.empty()) {
            continue;
        }
        std::string id = com::IdParser(type).namespace_("lua").toString();
        LuaFrxObject::Factory &fac = LuaFrxObject::Factory::instance();
        if (!fac.isRegistered(id)) {
            continue;
        }
        lua_pushnumber(lua, lua_index++);
        LuaFrxObject::Ptr lobj = fac.createAndPush(
            id,
            lua,
            mObj,
            map
        );
        if (!lobj) {
            lua_pushnil(lua);
        }
        lua_settable(lua, top);
    }
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
void LuaFrxView::addLuaFields(lua_State *lua, int index) {
    using boost::bind;
    Super::addLuaFields(lua, index);
    sambag::lua::registerFunctions<Functions,
        sambag::lua::TupleAccessor>
    (
        lua,
        boost::make_tuple(
            bind(&LuaFrxView::addProcessor, this, lua, _1),
            bind(&LuaFrxView::remove, this, lua),
            bind(&LuaFrxView::getObjects, this, lua),
            bind(&LuaFrxView::connect, this, lua)
        ),
        index
    );
}
//-----------------------------------------------------------------------------
LuaFrxView::LuaFrxView() {
}
//-----------------------------------------------------------------------------
fgc::FrxCircuidViewPtr LuaFrxView::getView() const {
    if (!editor) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "get: editor == NULL"
        );
    }
    fgc::FrxCircuidViewPtr res = editor->getCircuidView();
    if (!res) {
        SAMBAG_THROW(
            sambag::com::exceptions::IllegalStateException,
            "get: view == NULL"
        );
    }
    return res;
}
//-----------------------------------------------------------------------------
fgc::FrxCircuidViewPtr LuaFrxView::getView(lua_State *lua) const {
    try {
        return getView();
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, std::string("view is not available: ") + ex.what());
        return fgc::FrxCircuidViewPtr();
    } catch(...) {
        slua::pushLuaError(lua, "view is not available");
        return fgc::FrxCircuidViewPtr();
    }
}
//-----------------------------------------------------------------------------
LuaFrxView::Ptr
LuaFrxView::createAndPush(lua_State *lua,
    fgc::VstForxEditor *editor)
{
    Ptr res(new LuaFrxView());
    res->editor = editor;
    res->createLuaObject(lua, "frxView");
    return res;
}
}} // namespace(s)
