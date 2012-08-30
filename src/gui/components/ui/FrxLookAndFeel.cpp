/*
 * FrxLookAndFeel.cpp
 *
 *  Created on: Tue Aug 17 17:33:20 2012
 *      Author: Johannes Unger
 */

#include "FrxLookAndFeel.hpp"
#include <sambag/disco/components/ui/basic/BasicButtonUI.hpp>
#include <sambag/disco/components/ui/basic/BasicMenuItemUI.hpp>
#include <sambag/disco/components/ui/basic/BasicPopupMenuUI.hpp>
#include <sambag/disco/components/ui/basic/BasicLabelUI.hpp>
#include <sambag/disco/components/ui/basic/BasicMenuUI.hpp>
#include <sambag/disco/components/ui/basic/BasicScrollbarUI.hpp>
#include <sambag/disco/components/ui/basic/BasicPanelUI.hpp>
#include <sambag/disco/components/ui/basic/BasicScrollPaneUI.hpp>
#include <sambag/disco/components/Panel.hpp>
#include <sambag/disco/components/Button.hpp>
#include <sambag/disco/components/MenuItem.hpp>
#include <sambag/disco/components/PopupMenu.hpp>
#include <sambag/disco/components/Label.hpp>
#include <sambag/disco/components/Menu.hpp>
#include <sambag/disco/components/Scrollbar.hpp>
#include <sambag/disco/Geometry.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <sambag/disco/svg/HtmlColors.hpp>
#include <sambag/disco/components/Viewport.hpp>
#include <sambag/disco/components/ScrollPane.hpp>
#include <sambag/disco/components/Knob.hpp>
// Frx
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
	using namespace sambag::disco::components;
	using namespace sambag::disco::components::ui;
	using namespace sambag::disco::components::ui::basic;
	registerComponentUI<Button, BasicButtonUI<Button::Model> >();
	registerComponentUI<MenuItem, BasicMenuItemUI<MenuItem::Model> >();
	registerComponentUI<PopupMenu, BasicPopupMenuUI<PopupMenu::Model> >();
	registerComponentUI<Label, BasicLabelUI>();
	registerComponentUI<Menu, BasicMenuUI<Menu::Model> >();
	registerComponentUI<Scrollbar, BasicScrollbarUI<Scrollbar::Model> >();
	registerComponentUI<Panel, BasicPanelUI >();
	registerComponentUI<Viewport, BasicPanelUI >();
	registerComponentUI<ScrollPane, BasicScrollPaneUI>();
	// frx
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
	using namespace sambag::disco;
	using namespace sambag::disco::svg;
	using namespace sambag::disco::components;
	using namespace sambag::disco::components::ui;
	using namespace sambag::disco::components::ui::basic;
	UIManager &m = getUIManager();
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<global
	m.putProperty("global.background", HtmlColors::getColor("lightblue"));
	m.putProperty("global.foreground", HtmlColors::getColor("white"));
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<ScrollBar
	m.putProperty("ScrollBar.minimumThumbSize", Dimension(15., 15.));
	m.putProperty("ScrollBar.maximumThumbSize", Dimension(20., 20.));
	m.putProperty("ScrollBar.incrementButtonGap", Coordinate(5.));
	m.putProperty("ScrollBar.decrementButtonGap", Coordinate(5.));
	m.putProperty("ScrollBar.thumbHighlight", HtmlColors::getColor("lightblue"));
	m.putProperty("ScrollBar.thumbShadow", HtmlColors::getColor("darkgrey"));
	m.putProperty("ScrollBar.thumbDarkShadow", HtmlColors::getColor("black"));
	m.putProperty("ScrollBar.thumb", HtmlColors::getColor("grey"));
	m.putProperty("ScrollBar.track", HtmlColors::getColor("lightgrey"));
	m.putProperty("ScrollBar.trackHighlight", HtmlColors::getColor("lightblue"));
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<knobs
	m.putProperty("Knob.speedFactor", 0.5);
	m.putProperty("Knob.knobMode", std::string("linear"));
	//m.putProperty("Knob.knobMode",  std::string("circular"));
	//m.putProperty("Knob.knobMode",  std::string("relativeCircular"));

}

}}}} // namespace(s)
