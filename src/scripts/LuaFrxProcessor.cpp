/*
 * ============================================================================
 * LuaFrxProcessor.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */

#include "LuaFrxProcessor.hpp"
#include <sambag/com/Common.hpp>
#include <boost/foreach.hpp>
#include <gui/components/FrxIO.hpp>
#include "LuaFrxIO.hpp"
#include <gui/components/FrxCircuidView.hpp>
#include <exception>
#include "LuaFrxParameter.hpp"
#include <processing/IParameter.hpp>
#include <processing/IPluginAdapter.hpp>
#include <processing/ProcessorAdapter.hpp>
#include <processing/SerializationRegister.hpp>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaFrxProcessor
//=============================================================================
//-----------------------------------------------------------------------------
void LuaFrxProcessor::openCloseEditor(lua_State *lua) {
    using namespace frx::gui;
    using namespace frx::gui::components;
    try {
        FrxProcessorNode::Ptr obj =
            boost::dynamic_pointer_cast<FrxProcessorNode>(getViewObject());
        FrxCircuidView::Ptr view = obj->getFirstContainer<FrxCircuidView>();
        IFrxControl &frxctrl = getFrxControl(view);
        frxctrl.openClosePluginEditor(view, obj);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unkown error");
    }
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxProcessor::getInputs(lua_State *lua) {
    using namespace frx::gui;
    using namespace frx::gui::components;
    try {
        FrxProcessorNode::Ptr obj =
            boost::dynamic_pointer_cast<FrxProcessorNode>(getViewObject());
        IViewModelMap::Ptr map;
        map = getViewModelMap();
        const FrxProcessorNode::IOContainer &cont = obj->getInputs();
        lua_createtable(lua, cont.size(), 0);
        int top = lua_gettop(lua);
        int lua_index = 0;
        BOOST_FOREACH(FrxIO::Ptr x, obj->getInputs()) {
            lua_pushinteger(lua, ++lua_index);
            LuaFrxIO::createAndPush(lua,
                map->getModelObject(x), map, "frx.lua.io.Input");
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
slua::IgnoreReturn LuaFrxProcessor::getOutputs(lua_State *lua) {
    using namespace frx::gui;
    using namespace frx::gui::components;
    try {
        FrxProcessorNode::Ptr obj =
            boost::dynamic_pointer_cast<FrxProcessorNode>(getViewObject());
        IViewModelMap::Ptr map;
        map = getViewModelMap();
        const FrxProcessorNode::IOContainer &cont = obj->getOutputs();
        lua_createtable(lua, cont.size(), 0);
        int top = lua_gettop(lua);
        int lua_index = 0;
        BOOST_FOREACH(FrxIO::Ptr x, obj->getOutputs()) {
            lua_pushinteger(lua, ++lua_index);
            LuaFrxIO::createAndPush(lua,
                map->getModelObject(x), map, "frx.lua.io.Output");
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
slua::IgnoreReturn LuaFrxProcessor::getParameters(lua_State *lua) {
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
slua::IgnoreReturn LuaFrxProcessor::addInput(lua_State *lua, bool follow) {
    using namespace frx::gui;
    using namespace frx::gui::components;
    try {
        FrxProcessorNode::Ptr obj =
            boost::dynamic_pointer_cast<FrxProcessorNode>(getViewObject());
        FrxCircuidView::Ptr view = obj->getFirstContainer<FrxCircuidView>();
        if (!view) {
            lua_pushnil(lua);
            return slua::IgnoreReturn();
        }
        IFrxControl &frxctrl = getFrxControl(view);
        FrxComponent::Ptr newIn = frxctrl.addProcessorInput(view, obj, follow);
        if (!newIn) {
            lua_pushnil(lua);
            return slua::IgnoreReturn();
        }
        IViewModelMap::Ptr map;
        map = getViewModelMap();
        LuaFrxIO::createAndPush(lua, map->getModelObject(newIn), map, "frx.lua.io.Input");
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unkown error");
    }
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxProcessor::addOutput(lua_State *lua, bool follow) {
    using namespace frx::gui;
    using namespace frx::gui::components;
    try {
        FrxProcessorNode::Ptr obj =
            boost::dynamic_pointer_cast<FrxProcessorNode>(getViewObject());
        FrxCircuidView::Ptr view = obj->getFirstContainer<FrxCircuidView>();
        if (!view) {
            lua_pushnil(lua);
            return slua::IgnoreReturn();
        }
        IFrxControl &frxctrl = getFrxControl(view);
        FrxComponent::Ptr newOut = frxctrl.addProcessorOutput(view, obj, follow);
        if (!newOut) {
            lua_pushnil(lua);
            return slua::IgnoreReturn();
        }
        IViewModelMap::Ptr map;
        map = getViewModelMap();
        LuaFrxIO::createAndPush(lua, map->getModelObject(newOut), map, "frx.lua.io.Input");
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unkown error");
    }
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
int LuaFrxProcessor::getNumInputs(lua_State *lua) {
    using namespace frx::gui;
    using namespace frx::gui::components;
    try {
        FrxProcessorNode::Ptr obj =
            boost::dynamic_pointer_cast<FrxProcessorNode>(getViewObject());
        return obj->getInputs().size();
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unkown error");
    }
    return 0;
}
//-----------------------------------------------------------------------------
int LuaFrxProcessor::getNumOutputs(lua_State *lua) {
    using namespace frx::gui;
    using namespace frx::gui::components;
    try {
        FrxProcessorNode::Ptr obj =
            boost::dynamic_pointer_cast<FrxProcessorNode>(getViewObject());
        return obj->getOutputs().size();
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unkown error");
    }
    return 0;
}
//-----------------------------------------------------------------------------
int LuaFrxProcessor::getNumParameters(lua_State *lua) {
    using frx::processing::IParameter;
    using namespace frx::gui;
    using namespace frx::gui::components;
    try {
        ModelObject::Ptr obj = getModelObject();
        IViewModelMap::Ptr map;
        map = getViewModelMap();
        ModelObject::Parameters parameters;
        obj->getParameters("*", parameters);
        return (int)parameters.size();
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unkown error");
    }
    return -1;
}
//-----------------------------------------------------------------------------
std::string LuaFrxProcessor::getPluginLocation(lua_State *lua) {
    using namespace frx::gui;
    using namespace frx::gui::components;
    using namespace frx::processing;
    try {
        IPluginAdapter::Ptr obj =
            boost::dynamic_pointer_cast<IPluginAdapter>(getModelObject());
        if (!obj) {
            return "";
        }
        return obj->getLocation();
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unkown error");
    }
    return "";
}
//-----------------------------------------------------------------------------
std::string LuaFrxProcessor::serialize(lua_State *lua) {
	using namespace frx::gui;
	using namespace frx::gui::components;
	using namespace frx::processing;
	try {
		ProcessorAdapter::Ptr obj =
			boost::dynamic_pointer_cast<ProcessorAdapter>(getModelObject());
		if (!obj || !obj->supportsPresetSerialization()) {
			return "";
		}
		return obj->getPresetData();
	}
	catch (const std::exception &ex) {
		slua::pushLuaError(lua, ex.what());
	}
	catch (...) {
		slua::pushLuaError(lua, "unkown error");
	}
	return "";
}
//-----------------------------------------------------------------------------
void LuaFrxProcessor::deserialize(lua_State *lua, const std::string & data) {
	using namespace frx::gui;
	using namespace frx::gui::components;
	using namespace frx::processing;
	try {
		ProcessorAdapter::Ptr obj =
			boost::dynamic_pointer_cast<ProcessorAdapter>(getModelObject());
		if (!obj || !obj->supportsPresetSerialization()) {
			return;
		}
		obj->setPresetData(data);
	}
	catch (const std::exception &ex) {
		slua::pushLuaError(lua, ex.what());
	}
	catch (...) {
		slua::pushLuaError(lua, "unkown error");
	}
}
//-----------------------------------------------------------------------------
LuaFrxProcessor::LuaFrxProcessor() {
}
//-----------------------------------------------------------------------------
LuaFrxProcessor::Ptr
LuaFrxProcessor::createAndPush(lua_State *lua,
    ModelObject::Ptr obj, ViewModelMap::Ptr map, const std::string &typeId)
{
    Ptr res(new LuaFrxProcessor());
    res->setModelObject(obj);
    res->setViewModelMap(map);
    res->setTypeId(typeId);
    res->createLuaObject(lua, "lua_processor");
    return res;
}
//-----------------------------------------------------------------------------
std::string LuaFrxProcessor::sendMessage(lua_State *lua, const std::string &msg)
{
    using namespace frx::gui;
    using namespace frx::gui::components;
    using namespace frx::processing;
    try {
        IPluginAdapter::Ptr obj =
            boost::dynamic_pointer_cast<IPluginAdapter>(getModelObject());
        if (!obj) {
            return "";
        }
        return obj->sendMessage(msg);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unkown error");
    }
    return "";
}
}} // namespace(s)
