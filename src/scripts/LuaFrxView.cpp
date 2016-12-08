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
#include <gui/components/FrxPacket.hpp>
#include <gui/IFrxControl.hpp>
#include "LuaFrxProcessor.hpp"
#include "LuaFrxIO.hpp"
#include "LuaFrxConnection.hpp"
#include "LuaFrxParameterConnection.hpp"
#include "LuaFrxParameter.hpp"
#include "LuaModelObject.hpp"
#include <com/one4All.h>
#include <gui/components/FrxConcreteIO.hpp>
#include <processing/IParameter.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <sambag/disco/components/PopupMenu.hpp>
#include <sambag/disco/components/Label.hpp>
#include <gui/components/FrxMenuLabel.hpp>
#include "LuaFrxListWindow.hpp"
#include <sambag/disco/components/MenuSelectionManager.hpp>

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
        return addRelatedParameter(lua);
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
slua::IgnoreReturn LuaFrxView::addRelatedParameter(lua_State *lua) {
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = getView();
    IFrxControl &frxctrl = getFrxControl(view);
    IViewModelMap::Ptr map = getViewModelMap(view);
    // get related processor
    lua_getfield(lua, -1, "__related");
    if (!lua_isstring(lua, -1)) {
        throw std::runtime_error("no related object found");
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
        LuaFrxObject::Factory &fac = LuaFrxObject::Factory::instance();
        if (!fac.isRegistered(newId)) {
            lua_pushnil(lua);
            return slua::IgnoreReturn();
        }
        LuaFrxObject::Ptr lobj = fac.createAndPush(
            newId,
            lua,
            map->getModelObject(cn),
            map
        );
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
boost::tuple<float,float> LuaFrxView::getLocation(lua_State *lua) {
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
    SAMBAG_BEGIN_SYNCHRONIZED(vp->getTreeLock())
        vp->setViewPosition(sd::Point2D(x, y));
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
boost::tuple<float,float> LuaFrxView::getSize(lua_State *lua) {
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
    FrxEntryNode::Ptr entry;
    FrxCircuidViewPtr view = getView(lua);
    IViewModelMap::Ptr map = getViewModelMap(view);
    if (!view || !map) {
        //not here because getview pushes error already: lua_pushnil(lua);
        return slua::IgnoreReturn();
    }
    std::string id("1");
    if (lua_isstring(lua, -1)) {
        id = std::string(lua_tostring(lua,-1));
        lua_pop(lua, 1);
    }
	const FrxCircuidView::Components &comps = view->getContentPane()->getComponents();
	BOOST_FOREACH(AComponentPtr c, comps) {
		entry = boost::dynamic_pointer_cast<FrxEntryNode>(c);
        if (!entry) {
            continue;
        }
        if (id.length()>0 && id==entry->getDisplayText()) {
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
	FrxExitNode::Ptr exit;
    FrxCircuidViewPtr view = getView(lua);
    IViewModelMap::Ptr map = getViewModelMap(view);
    if (!view || !map) {
        return slua::IgnoreReturn();
    }
    std::string id("1");
    if (lua_isstring(lua, -1)) {
        id = std::string(lua_tostring(lua,-1));
    }
	const FrxCircuidView::Components &comps = view->getContentPane()->getComponents();
	BOOST_FOREACH(AComponentPtr c, comps) {
		exit = boost::dynamic_pointer_cast<FrxExitNode>(c);
        if (!exit) {
            continue;
        }
        if (id.length()>0 && id==exit->getDisplayText()) {
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
void LuaFrxView::addViewListener(lua_State *lua, const std::string &listener) {
    initListenersIfNeccessary(lua);
    luaViewListener.insert(listener);
}
//-----------------------------------------------------------------------------
void LuaFrxView::removeViewListener(lua_State *lua, const std::string &listener) {
    luaViewListener.erase(listener);
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
} // namespace
//-----------------------------------------------------------------------------
void LuaFrxView::onMenu(lua_State *lua, const std::string &cmd) {
    fgc::FrxCircuidView::Ptr view = getView();
    SAMBAG_BEGIN_SYNCHRONIZED(getLock(lua))
        try {
            slua::executeString(lua, cmd);
        } catch(const sambag::lua::ExecutionFailed &ex) {
            view->errorMessage(cmd + " failed: " + ex.errMsg);
        } catch(...) {
            view->errorMessage(cmd + " failed: unkown reason");
        }
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
void LuaFrxView::addMenuEntry(sdc::PopupMenuPtr res, lua_State *lua, int index)
{
    using namespace sambag::disco::components;
    lua_pushnil(lua); /* first key */
    --index;
    std::string name, action;
    Menu::Ptr smenu;
    while (lua_next(lua, index) != 0) { // -1 == key index, -2 == value index
        std::string key, value;
        if (lua_isstring(lua, -1) == 1) { // value
            slua::get(value, lua, -1);
        }
        if (lua_istable(lua, -1)==1) { // is table
            // create sub menu
            smenu = Menu::create();
            lua_pushnil(lua); /* first key */
            while (lua_next(lua, -2) != 0) {
                if (lua_istable(lua, -1)==1) {
                    addMenuEntry(smenu->getPopupMenu(), lua, -1);
                }
                lua_pop(lua, 1);
            }
        }
        if (lua_type(lua, -2) != LUA_TSTRING) { // ignore non string keys
            lua_pop(lua, 1);
            continue;
        }
        slua::get(key, lua, -2);
        lua_pop(lua, 1);
        if (key=="name") {
            name = value;
        }
        if (key=="action") {
            action = value;
        }
    }
    if (name.empty()) {
        return;
    }
    if (smenu) {
        smenu->setText(name);
        res->add(smenu);
        return;
    }
    // add menu item
    if (action.length()==0) {
        fgc::FrxMenuLabel::Ptr label = fgc::FrxMenuLabel::create();
        label->setText(name);
        res->add(label);
        return;
    }
    MenuItem::Ptr item = MenuItem::create();
    item->EventSender<sdc::events::ActionEvent>::addTrackedEventListener(
        boost::bind(&LuaFrxView::onMenu, this, lua, action),  shared_from_this());
    item->setText(name);
    res->add(item);
}
//-----------------------------------------------------------------------------
void LuaFrxView::setMenu(lua_State *lua) {
    using namespace sambag::disco::components;
    try {
        using namespace sambag::disco::components;
        PopupMenuPtr res = PopupMenu::create();
        // iterate through menu table
        int index = -1;
        if (lua_istable(lua, index)!=1) {
            throw std::runtime_error("invalid value");
        }
        lua_pushnil(lua); /* first key */
        --index;
        while (lua_next(lua, index) != 0) {
            if (lua_istable(lua, -1)==1) {
                addMenuEntry(res, lua, -1);
            }
            lua_pop(lua, 1);
        }
        getView()->setComponentPopupMenu(res);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
}
//-----------------------------------------------------------------------------
void LuaFrxView::showMenu(lua_State *lua) {
    using namespace sambag::disco::components;
    try {
        using namespace sambag::disco::components;
        typedef boost::weak_ptr<PopupMenu> PopupMenuWPtr;
        PopupMenuWPtr wres;
        PopupMenuPtr res;
        AComponentPtr invoker = getView();
        invoker->getClientProperty("lua.addmenu", wres);
        res = wres.lock();
        if (res) {
            res->hidePopup();
        }
        
        wres = res = PopupMenu::create();
        invoker->putClientProperty("lua.addmenu", wres);
        // iterate through menu table
        int index = -1;
        if (lua_istable(lua, index)!=1) {
            throw std::runtime_error("invalid value");
        }
        lua_pushnil(lua); /* first key */
        --index;
        while (lua_next(lua, index) != 0) {
            if (lua_istable(lua, -1)==1) {
                addMenuEntry(res, lua, -1);
            }
            lua_pop(lua, 1);
        }
        lua_pop(lua, 1);
        // get pos
        boost::tuple<int, int> pos;
        sambag::lua::pop(lua, pos);
        MenuSelectionManager &m = MenuSelectionManager::defaultManager();
		m.clearSelectedPath();
		IMenuElement::MenuElements p;
		p.push_back(res);
		m.setSelectedPath(p);
		res->setInvoker(invoker);
        res->showPopup(sd::Point2D(boost::get<1>(pos), boost::get<0>(pos)));
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
}
//-----------------------------------------------------------------------------
void LuaFrxView::closeMenu(lua_State *lua) {
    using namespace sambag::disco::components;
    typedef boost::weak_ptr<PopupMenu> PopupMenuWPtr;
    try {
        AComponentPtr invoker = getView();
        PopupMenuWPtr wres;
        PopupMenuPtr res;
        invoker->getClientProperty("lua.addmenu", wres);
        res = wres.lock();
        if (res) {
            res->hidePopup();
        }
        invoker->putClientProperty("lua.addmenu", PopupMenuWPtr());
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
}
//-----------------------------------------------------------------------------
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
slua::IgnoreReturn LuaFrxView::getContextObject(lua_State *lua) {
	using namespace frx::gui::components;
	using namespace sambag::disco::components;
    using namespace frx::gui;
    try {
        FrxCircuidView::Ptr view = getView();
        IViewModelMap::Ptr map = getViewModelMap(view);
        fgc::FrxComponentWPtr wobj;
        view->getClientProperty("popupcontext", wobj);
        fgc::FrxComponentPtr obj = wobj.lock();
        if (!obj) {
            lua_pushnil(lua);
            return slua::IgnoreReturn();
        }
        std::string type = obj->getTypeId();
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
            map->getModelObject(obj),
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
void LuaFrxView::onViewEvent(lua_State *lua, const fgc::FrxCircuidViewEvent &ev)
{
	using namespace frx::gui::components;
	using namespace sambag::disco::components;
    using namespace frx::gui;
    std::string evtype;
    if (ev.type == fgc::FrxCircuidViewEvent::ComponentAdded) {
        evtype = "object added";
    }
    if (ev.type == fgc::FrxCircuidViewEvent::OnSerializing) {
        evtype = "on saving view state";
    }
    if (ev.type == fgc::FrxCircuidViewEvent::OnComponentMenuRequest) {
        evtype = "requesting context menu";
    }
    if (ev.type == fgc::FrxCircuidViewEvent::OnDeserializing) {
        // we can't catch this event because while deserializing
        // the script isn't running.
    }
    if (evtype.empty()) {
        return;
    }
    SAMBAG_BEGIN_SYNCHRONIZED(getLock(lua))
        try {
            std::string id;
            FrxCircuidViewPtr view;
            IViewModelMap::Ptr map;
            if (ev.component) {
                // prepare lua object
                view = getView();
                map = getViewModelMap(view);
                std::string type = ev.component->getTypeId();
                id = com::IdParser(type).namespace_("lua").toString();
            }
            // determine event type
            // notify
            LuaFrxObject::Factory &fac = LuaFrxObject::Factory::instance();
            BOOST_FOREACH(const std::string &x, luaViewListener) {
                lua_getglobal(lua, x.c_str());
                // push event type
                lua_pushstring(lua, evtype.c_str());
                if (fac.isRegistered(id)) {
                    // push lua object
                    LuaFrxObject::Ptr lobj = fac.createAndPush(
                        id,
                        lua,
                        map->getModelObject(ev.component),
                        map
                    );
                } else {
                    lua_pushnil(lua);
                }
                if (lua_pcall(lua, 2, 0, 0)!=0) {
                    SAMBAG_LOG_ERR<<lua_tostring(lua, -1);
                }
            }
        } catch(...) {
        }
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
void LuaFrxView::initListenersIfNeccessary(lua_State *lua) {
    fgc::FrxCircuidViewPtr view = listenerInstalled.lock();
    if (view) {
        if (view==editor->getCircuidView()) {
            //already installed
            return;
        }
    }
    listenerInstalled = view = getView();
    view->sce::EventSender<fgc::FrxCircuidViewEvent>::addTrackedEventListener(
        boost::bind(&LuaFrxView::onViewEvent, this, lua, _2),
        shared_from_this()
    );
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::createListWindow(lua_State *lua) {
    try {
        fgc::FrxCircuidViewPtr view = getView();
        sdc::Window::Ptr win = view->getFirstContainer<sdc::Window>();
        LuaFrxListWindow::Ptr res = LuaFrxListWindow::createAndPush(lua, win);
        return slua::IgnoreReturn();
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, std::string("view is not available: ") + ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "view is not available");
    }
    return slua::IgnoreReturn();
}
//-----------------------------------------------------------------------------
void LuaFrxView::addToSelection(lua_State *lua) {
    try {
        LuaFrxObject::Ptr obj = LuaFrxObject::getFromLuaStack(lua, -1);
        if (!obj) {
            throw std::runtime_error("no valid object");
        }
        fgc::FrxCircuidViewPtr view = getView();
        fgc::FrxSelection::Ptr sel = view->getSelection();
        fgc::FrxSelection::ContentContainer &c = sel->getContent();
        c.push_back(obj->getViewObject());
        sel->updateBounds();
        sel->setVisible(true);
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "unknown error");
    }
}
//-----------------------------------------------------------------------------
void LuaFrxView::clearSelection(lua_State *lua) {
    try {
        fgc::FrxCircuidViewPtr view = getView();
        fgc::FrxSelection::Ptr sel = view->getSelection();
        sel->clearContent();
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, std::string("view is not available: ") + ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "view is not available");
    }
}
//-----------------------------------------------------------------------------
void LuaFrxView::packSelection(lua_State *lua, const std::string & packetName) {
	try {
		fgc::FrxCircuidViewPtr view = getView();
		fgc::FrxSelection::Ptr sel = view->getSelection();
		fgc::FrxPacket::Ptr packet = fgc::FrxPacket::create(view, sel->getContent());
		sd::Point2D p = sel->getLocation();
		p.x(p.x() + sel->getWidth() / 2. - packet->getWidth() / 2.);
		p.y(p.y() + sel->getHeight() / 2. - packet->getHeight() / 2.);
		packet->setLocation(p);
		packet->setName(packetName);
		sel->clearContent();
	}
	catch (const std::exception &ex) {
		slua::pushLuaError(lua, std::string("view is not available: ") + ex.what());
	}
	catch (...) {
		slua::pushLuaError(lua, "view is not available");
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
