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
#include <gui/components/FrxConnection.hpp>
#include <gui/components/FrxProcessorNode.hpp>
#include <gui/components/FrxParameter.hpp>
#include <gui/IFrxControl.hpp>
#include "LuaFrxProcessor.hpp"
#include "LuaFrxIO.hpp"
#include "LuaFrxConnection.hpp"
#include "LuaFrxParameter.hpp"
#include "LuaModelObject.hpp"
#include <com/one4All.h>
#include <gui/components/FrxConcreteIO.hpp>
#include <processing/IParameter.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

namespace frx { namespace scripts {
//=============================================================================
//  Class LuaFrxView
//=============================================================================
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::add(lua_State *lua)
{
    try {
        // determine what to add
        if (lua_istable(lua, -1)) {
            return addObject(lua);
        }
        if (lua_isstring(lua, -1)) {
            boost::tuple<std::string> id;
            slua::pop(lua, id);
            return addProcessor(lua, boost::get<0>(id));
        }
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::addObject(lua_State *lua)
{
    lua_getfield(lua, -1, "__frxtype");
    if (!lua_isstring(lua, -1)) {
        throw std::runtime_error("unknown table type");
    }
    boost::tuple<std::string> id;
    slua::pop(lua, id);
    if (boost::get<0>(id)=="frx.lua.parameter.StdKnob") {
        return addProcessorParameter(lua);
    }
    throw std::runtime_error("cannot add " + boost::get<0>(id));
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::addProcessor(lua_State *lua, const std::string &id)
{
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = getView();
	IFrxComponentFactory &fac = getComponentFactory(view);
	IFrxControl &frxctrl = getFrxControl(view);
    IViewModelMap::Ptr map = getViewModelMap(view);
	FrxProcessorNodePtr res;
    res = fac.getProcessorCreator(id)(view);
	if (!res) {
        slua::pushLuaError(lua, "creation of " + id + " failed.");
		return slua::IgnoreReturn();
	}
  	frxctrl.addProcessorToView(view, res);
    com::IdParser newId(res->getTypeId());
    LuaFrxProcessor::createAndPush(lua,
        map->getModelObject(res), map,
        newId.namespace_("lua").numInputs(-1).numOutputs(-1).details("").toString());
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::addProcessorParameter(lua_State *lua) {
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = getView();
    IFrxControl &frxctrl = getFrxControl(view);
    IViewModelMap::Ptr map = getViewModelMap(view);
    // get related processor
    lua_getfield(lua, -1, "__processor");
    if (!lua_isstring(lua, -1)) {
        throw std::runtime_error("no related processor found");
    }
    boost::tuple<std::string> prId;
    slua::pop(lua, prId);
    LuaFrxObject::Ptr fobj = LuaFrxObject::getByUId(boost::get<0>(prId));
    // get object
    LuaFrxObject::Ptr vobj = LuaFrxObject::getFromLuaStack(lua, -1);
    using frx::processing::IParameter;
    IParameter::Ptr par =
        boost::dynamic_pointer_cast<IParameter>(vobj->getModelObject());
    SAMBAG_ASSERT(par);
    FrxComponent::Ptr res =
        frxctrl.addRelatedKnobToView(view, fobj->getViewObject(), par);
    
    return slua::IgnoreReturn();
}

//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::addKnob(lua_State *lua)
{
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = getView(lua);
    if (!view) {
        //not here because getview pushes error already: lua_pushnil(lua)
        return slua::IgnoreReturn();
    }
	IFrxComponentFactory &fac = getComponentFactory(view);
	IFrxControl &frxctrl = getFrxControl(view);
    IViewModelMap::Ptr map = getViewModelMap(view);
	fgc::FrxParameter::Ptr res;
    try {
        res = fac.getFreeParameterCreator()(view);
    } catch (...) {
    }
	if (!res) {
        slua::pushLuaError(lua, "creation of knob failed.");
		return slua::IgnoreReturn();
	}
  	frxctrl.addParameterToView(view, res);
    LuaFrxParameter::createAndPush(lua,
        map->getModelObject(res), map, "frx.lua.parameter.StdKnob");
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::addHostKnob(lua_State *lua, int index)
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
	fgc::FrxParameter::Ptr res;
    try {
        res = fac.getHostParameterCreator()(view, index);
    } catch (...) {
    }
	if (!res) {
        slua::pushLuaError(lua, "creation of knob failed.");
		return slua::IgnoreReturn();
	}
  	frxctrl.addParameterToView(view, res);
    LuaFrxParameter::createAndPush(lua,
        map->getModelObject(res), map, "frx.lua.parameter.StdKnob");
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
        // wenns nich geht dann gehts halt nich -- for convenience
    } catch(...) {
    }
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::connect(lua_State *lua) {
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = getView(lua);
    IViewModelMap::Ptr map = getViewModelMap(view);
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
            lua_pushnil(lua);
            return slua::IgnoreReturn();
        }
        FrxConnection::Ptr cn = frxctrl.connect(view, na, nb);
        if (!cn) {
            lua_pushnil(lua);
            return slua::IgnoreReturn();
        }
        std::string newId = com::IdParser(cn->getTypeId()).namespace_("lua").toString();
        LuaFrxConnection::createAndPush(lua, map->getModelObject(cn), map, newId);
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
    try {
        FrxCircuidViewPtr view = getView();
        const FrxCircuidView::Components &comps =
            view->getContentPane()->getComponents();
        
        pushComponents(lua, comps);
        
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unkown error");
    }
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::getSelectedObjects(lua_State *lua) {
    using namespace frx::gui;
	using namespace frx::gui::components;
    using namespace sambag::disco::components;
    try {
        FrxCircuidViewPtr view = getView();
        const FrxSelection::ContentContainer & c =
            view->getSelection()->getContent();
        std::vector<AComponent::Ptr> vec;
        vec.reserve(c.size());
        BOOST_FOREACH(AComponent::WPtr x, c) {
            AComponent::Ptr ptr = x.lock();
            if (!ptr) {
                continue;
            }
            vec.push_back(ptr);
        }
        pushComponents(lua, vec);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unkown error");
    }
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
boost::tuple<float,float> LuaFrxView::getLocation(lua_State *lua) const {
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = getView(lua);
    if (!view) {
        return boost::make_tuple(0.f, 0.f);
    }
    sdc::Viewport::Ptr vp = view->getViewport();
    if (!vp) {
        return boost::make_tuple(0.f, 0.f);
    }
    sd::Point2D pos = vp->getViewPosition();
    return boost::make_tuple(pos.x(), pos.y());
}
//-----------------------------------------------------------------------------
void LuaFrxView::setLocation(lua_State *lua, float x, float y) {
    using namespace frx::gui;
    using namespace frx::gui::components;
    
    FrxCircuidViewPtr view = getView(lua);
    if (!view) {
        return;
    }
    sdc::Viewport::Ptr vp = view->getViewport();
    if (!vp) {
        return;
    }
    vp->setViewPosition(sd::Point2D(x, y));
}
//-----------------------------------------------------------------------------
boost::tuple<float,float> LuaFrxView::getSize(lua_State *lua) const {
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = getView(lua);
    if (!view) {
        return boost::make_tuple(0.f, 0.f);
    }
    sdc::Window::Ptr win = view->getFirstContainer<sdc::Window>();
    if (!win) {
        return boost::make_tuple(0.f, 0.f);
    }
    sd::Dimension d = win->getWindowSize();
    return boost::make_tuple(d.width(), d.height());
}
//-----------------------------------------------------------------------------
void LuaFrxView::setSize(lua_State *lua, float x, float y) {
   using namespace frx::gui;
    using namespace frx::gui::components;
    FrxCircuidViewPtr view = getView(lua);
    if (!view) {
        return;
    }
    view->requestEditorResize(sd::Dimension(x,y));
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::getEntry(lua_State *lua) {
	using namespace frx::gui::components; 
	using namespace sambag::disco::components;
    using namespace frx::gui;
    FrxNode::Ptr entry;
    FrxCircuidViewPtr view = getView(lua);
    IViewModelMap::Ptr map = getViewModelMap(view);
    if (!view || !map) {
        //not here because getview pushes error already: lua_pushnil(lua);
        return slua::IgnoreReturn();
    }
	const FrxCircuidView::Components &comps = view->getContentPane()->getComponents();
	BOOST_FOREACH(AComponentPtr c, comps) {
		entry = boost::dynamic_pointer_cast<FrxEntryNode>(c);
        if (entry) {
            break;
        }
	}
    if (!entry) {
        lua_pushnil(lua);
        return slua::IgnoreReturn();
    }
    LuaFrxIO::createAndPush(lua,
        map->getModelObject(entry), map, "frx.lua.io.Entry");
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::getExit(lua_State *lua) {
	using namespace frx::gui::components; 
	using namespace sambag::disco::components;
    using namespace frx::gui;
	FrxNode::Ptr exit;
    FrxCircuidViewPtr view = getView(lua);
    IViewModelMap::Ptr map = getViewModelMap(view);
    if (!view || !map) {
        return slua::IgnoreReturn();
    }
	const FrxCircuidView::Components &comps = view->getContentPane()->getComponents();
	BOOST_FOREACH(AComponentPtr c, comps) {
		exit = boost::dynamic_pointer_cast<FrxExitNode>(c);
        if (exit) {
            break;
        }
	}
    if (!exit) {
        lua_pushnil(lua);
        return slua::IgnoreReturn();
    }
    LuaFrxIO::createAndPush(lua,
        map->getModelObject(exit), map, "frx.lua.io.Exit");
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::getByName(lua_State *lua, const std::string &name)
{
	using namespace frx::gui::components;
	using namespace sambag::disco::components;
    using namespace frx::gui;
    try {
        FrxComponent::Ptr res;
        FrxCircuidViewPtr view = getView();
        IViewModelMap::Ptr map = getViewModelMap(view);
        const FrxCircuidView::Components &comps = view->getContentPane()->getComponents();
        BOOST_FOREACH(AComponentPtr c, comps) {
            FrxComponent::Ptr x = boost::dynamic_pointer_cast<FrxComponent>(c);
            if (!x) {
                continue;
            }
            if(x->getName() == name) {
                res = x;
                break;
            }
        }
        if (!res) {
            lua_pushnil(lua);
            return slua::IgnoreReturn();
        }
        std::string type = res->getTypeId();
        if (type.empty()) {
            lua_pushnil(lua);
            return slua::IgnoreReturn();
        }
        std::string id = com::IdParser(type).namespace_("lua").toString();
        LuaFrxObject::Factory &fac = LuaFrxObject::Factory::instance();
        if (!fac.isRegistered(id)) {
            lua_pushnil(lua);
            return slua::IgnoreReturn();
        }
        LuaFrxObject::Ptr lobj = fac.createAndPush(
            id,
            lua,
            map->getModelObject(res),
            map
        );
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
namespace {
    bool _matchType(const std::string &_a, const std::string &_b) {
        com::IdParser ia(_a);
        // remove detail and io
        std::string a = ia.details("").numInputs(-1).numOutputs(-1).toString();
        // convert b into regex
        std::string b = "frx.gui."+_b;
        b = boost::algorithm::replace_all_copy(b, ".", "\\.");
        b = boost::algorithm::replace_all_copy(b, "*", ".*?");
        return boost::regex_match(a, boost::regex(b));
    }
}
slua::IgnoreReturn LuaFrxView::getByType(lua_State *lua, const std::string &serachid)
{
	using namespace frx::gui::components;
	using namespace sambag::disco::components;
    using namespace frx::gui;
    try {
        FrxCircuidViewPtr view = getView();
        const FrxCircuidView::Components &comps =
            view->getContentPane()->getComponents();
        std::vector<sdc::AComponent::Ptr> res;
        std::string compType;
        BOOST_FOREACH(AComponentPtr c, comps) {
            FrxComponent::Ptr x = boost::dynamic_pointer_cast<FrxComponent>(c);
            if (!x) {
                continue;
            }
            compType = x->getTypeId();
            if(!_matchType(compType, serachid)) {
                continue;
            }
            res.push_back(x);
        }
        pushComponents(lua, res);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
void LuaFrxView::addLuaFields(lua_State *lua, int index) {
    using boost::bind;
    Super::addLuaFields(lua, index);
    
    // 1-10
    sambag::lua::registerClassFunctions<Functions1,
        sambag::lua::TupleAccessor>
    (
        lua,
        boost::make_tuple(
            bind(&LuaFrxView::add, this, lua),
            bind(&LuaFrxView::remove, this, lua),
            bind(&LuaFrxView::getObjects, this, lua),
            bind(&LuaFrxView::connect, this, lua),
            bind(&LuaFrxView::addKnob, this, lua),
            bind(&LuaFrxView::addHostKnob, this, lua, _1),
            bind(&LuaFrxView::getLocation, this, lua),
            bind(&LuaFrxView::setLocation, this, lua, _1, _2),
            bind(&LuaFrxView::getSize, this, lua),
            bind(&LuaFrxView::setSize, this, lua, _1, _2)
        ),
        index,
        getUId()
    );

    // 10-20
    sambag::lua::registerClassFunctions<Functions2,
        sambag::lua::TupleAccessor>
    (
        lua,
        boost::make_tuple(
            bind(&LuaFrxView::getEntry, this, lua),
            bind(&LuaFrxView::getExit, this, lua),
            bind(&LuaFrxView::getByName, this, lua, _1),
            bind(&LuaFrxView::getByType, this, lua, _1),
            bind(&LuaFrxView::getSelectedObjects, this, lua)
        ),
        index,
        getUId()
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
