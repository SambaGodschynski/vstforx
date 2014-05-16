/*
 * FrxPluginEditor.cpp
 *
 *  Created on: Tue Nov  6 10:27:25 2012
 *      Author: Johannes Unger
 */

#include "FrxPluginEditor.hpp"
#include <sambag/disco/components/windowImpl/WindowFlags.hpp>
namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxPluginEditor
//=============================================================================
//-----------------------------------------------------------------------------
void FrxPluginEditor::postConstructor() {
	Super::postConstructor();
    if (isRaw) {
        getContentPane()->setOpaque(false);
        getWindowImpl()->setFlag(sdc::WindowFlags::WND_RAW, true);
    }
}
//-----------------------------------------------------------------------------
FrxPluginEditor::Ptr
FrxPluginEditor::create(sdc::Window::Ptr parentWindow, bool isRaw)
{
    Ptr res(new FrxPluginEditor(parentWindow));
    res->self = res;
    res->isRaw = isRaw;
    res->postConstructor();
    res->initWindow();
    return res;
}
//-----------------------------------------------------------------------------
FrxPluginEditor::Ptr
FrxPluginEditor::create(sdc::AWindowImpl::Ptr winImpl, sdc::Window::Ptr parentWindow)
{
    Ptr res(new FrxPluginEditor(winImpl, parentWindow));
    res->self = res;
    res->isRaw = true;
    res->postConstructor();
    res->initWindow();
    return res;
}
//-----------------------------------------------------------------------------
FrxPluginEditor::~FrxPluginEditor() {
}
}}} // namespace(s)
