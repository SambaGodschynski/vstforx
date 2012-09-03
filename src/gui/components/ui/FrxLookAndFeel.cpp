/*
 * FrxLookAndFeel.cpp
 *
 *  Created on: Tue Aug 17 17:33:20 2012
 *      Author: Johannes Unger
 */

#include "FrxLookAndFeel.hpp"
#include <sambag/disco/components/ui/UIManager.hpp>
#include <sambag/disco/svg/HtmlColors.hpp>

#include <gui/components/FrxComponent.hpp>
#include <gui/components/ui/FrxComponentUI.hpp>
#include <gui/components/FrxConcreteProcessor.hpp>
#include <gui/components/ui/FrxProcessorNodeUI.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/ui/FrxCircuidViewUI.hpp>
#include <gui/components/FrxConcreteConnections.hpp>
#include <gui/components/ui/FrxConnectionUI.hpp>
#include <gui/components/FrxConcreteIO.hpp>
#include <gui/components/ui/FrxIOUI.hpp>
#include <gui/components/FrxSelection.hpp>
#include <gui/components/ui/FrxSelectionUI.hpp>
#include <gui/components/FrxConcreteParameter.hpp>
#include <gui/components/ui/FrxParameterUI.hpp>



namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxLookAndFeel
//=============================================================================
//-----------------------------------------------------------------------------
FrxLookAndFeel::FrxLookAndFeel() {
	installDefaults(); // <= always before installComponents
	installComponents();
}
//-----------------------------------------------------------------------------
void FrxLookAndFeel::installComponents() {
	Super::installComponents();
	namespace fgc = frx::gui::components;
	namespace fgcu = fgc::ui;
	// view
	registerComponentUI<fgc::FrxCircuidView, fgcu::FrxCircuidViewUI>();
	// processor nodes
	registerComponentUI<fgc::FrxPluginNode, fgcu::FrxProcessorNodeUI<FrxPluginNode::ProcessorType> >();
	// parameter components
	registerComponentUI<sdc::Knob, FrxParameterUI<FrxStdKnob::ControllerType> >();
	// connections
	registerComponentUI<fgc::IOCn, fgcu::FrxConnectionUI<fgc::IOCn::ConnectionType> >();
	registerComponentUI<fgc::ProcessorInputCn, fgcu::FrxConnectionUI<fgc::ProcessorInputCn::ConnectionType> >();
	registerComponentUI<fgc::ProcessorOutputCn, fgcu::FrxConnectionUI<fgc::ProcessorOutputCn::ConnectionType> >();
	// io's
	registerComponentUI<fgc::FrxInputNode, fgcu::FrxIOUI<fgc::FrxInputNode::IOType> >();
	registerComponentUI<fgc::FrxOutputNode, fgcu::FrxIOUI<fgc::FrxOutputNode::IOType> >();
	registerComponentUI<fgc::FrxEntryNode, fgcu::FrxIOUI<fgc::FrxEntryNode::IOType> >();
	registerComponentUI<fgc::FrxExitNode, fgcu::FrxIOUI<fgc::FrxExitNode::IOType> >();
	// misc
	registerComponentUI<fgc::FrxSelection, fgcu::FrxSelectionUI>();
	

}
//-----------------------------------------------------------------------------
void FrxLookAndFeel::installDefaults() {
	Super::installDefaults();
	using namespace sambag::disco;
	using namespace sambag::disco::svg;
	using namespace sambag::disco::components;
	using namespace sambag::disco::components::ui;
	using namespace sambag::disco::components::ui::basic;
	UIManager &m = getUIManager();
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<global
	m.putProperty("global.background", HtmlColors::getColor("lightblue"));
	m.putProperty("global.foreground", HtmlColors::getColor("white"));
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<knobs
	m.putProperty("FrxStdKnob.size", Dimension(25., 25.));
	m.putProperty("Knob.mode", std::string("linear"));

}

}}}} // namespace(s)
