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
	getContentPane()->setOpaque(false);
	getWindowImpl()->setFlag(sdc::WindowFlags::WND_RAW, true);
}

}}} // namespace(s)
