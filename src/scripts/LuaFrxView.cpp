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


namespace frx { namespace scripts {
//=============================================================================
//  Class LuaFrxView
//=============================================================================
//-----------------------------------------------------------------------------
slua::IgnoreReturn LuaFrxView::addProcessor(lua_State *lua, const std::string &id) {
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
        IFrxComponentFactory &fac = getComponentFactory(view);
        IFrxControl &frxctrl = getFrxControl(view);
        frxctrl.removeComponent(view, obj->getViewObject());
    } catch(const std::exception &ex) {
        slua::pushLuaError(lua, std::string("removing failed: ") + ex.what());
    } catch(...) {
        slua::pushLuaError(lua, "removing failed: unkown error");
    }
}
//-----------------------------------------------------------------------------
slua::IgnoreReturn getObjects(lua_State *lua) {
    using namespace frx::gui;
	using namespace frx::gui::components;
    FrxCircuidViewPtr view = getView(lua);
    if (!view) {
        return;
    }
    
    using namespace frx::gui::components; 
	using namespace sambag::disco::components;
	
	const FrxCircuidView::Components &comps = view->getContentPane()->getComponents();
	BOOST_FOREACH(AComponentPtr c, comps) {
		FrxNode::Ptr fc = 
			boost::dynamic_pointer_cast<FrxProcessorNode>(c);
		if (!fc) {
			continue;
		}
		res.push_back(ctrl->getLuaPtr(fc));
	}


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
            bind(&LuaFrxView::removeProcessor, this, lua)
            bind(&LuaFrxView::getObjects, this, lua)
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
