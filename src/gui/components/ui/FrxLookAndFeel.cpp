/*
 * FrxLookAndFeel.cpp
 *
 *  Created on: Tue Aug 17 17:33:20 2012
 *      Author: Johannes Unger
 */

#include "FrxLookAndFeel.hpp"
#include <sambag/disco/components/ui/UIManager.hpp>
#include <sambag/disco/svg/HtmlColors.hpp>
#include <sambag/disco/svg/StyleParser.hpp>
#include <sambag/disco/components/ColumnBrowser.hpp>
#include <sambag/disco/components/List.hpp>
#include <sambag/disco/components/ColumnView.hpp>
#include <sambag/disco/components/ui/basic/BasicColumnViewUI.hpp>

#include <gui/FrxControl.hpp>
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
#include <gui/components/FrxHover.hpp>
#include <gui/components/ui/FrxHoverUI.hpp>
#include <gui/components/ui/FrxSelectionUI.hpp>
#include <gui/components/FrxConcreteParameter.hpp>
#include <gui/components/ui/FrxParameterUI.hpp>
#include <gui/components/FrxColumnBrowser.hpp>
#include <gui/components/FrxParameterLabel.hpp>
#include <gui/components/ui/FrxParameterLabelUI.hpp>
#include <gui/components/ui/FrxBrowserListUI.hpp>

#include <sambag/disco/FileResourceManager.hpp>
#include <sambag/disco/IPattern.hpp>
#include <sambag/math/Matrix.hpp>

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
	registerComponentUI<fgc::FrxPluginNode, 
		fgcu::FrxProcessorNodeUI<FrxPluginNode::ProcessorType> >();
	registerComponentUI<fgc::FrxVolumeNode, 
		fgcu::FrxProcessorNodeUI<FrxVolumeNode::ProcessorType> >();
	registerComponentUI<fgc::FrxPanNode, 
		fgcu::FrxProcessorNodeUI<FrxPanNode::ProcessorType> >();
	registerComponentUI<fgc::FrxInStepNode, 
		fgcu::FrxProcessorNodeUI<FrxInStepNode::ProcessorType> >();
	registerComponentUI<fgc::FrxOutStepNode, 
		fgcu::FrxProcessorNodeUI<FrxOutStepNode::ProcessorType> >();
	registerComponentUI<fgc::FrxInSwitchNode, 
		fgcu::FrxProcessorNodeUI<FrxInSwitchNode::ProcessorType> >();
	registerComponentUI<fgc::FrxOutSwitchNode, 
		fgcu::FrxProcessorNodeUI<FrxOutSwitchNode::ProcessorType> >();
	registerComponentUI<fgc::FrxADSRNode, 
		fgcu::FrxProcessorNodeUI<FrxADSRNode::ProcessorType> >();
	registerComponentUI<fgc::FrxPeakTrackerNode, 
		fgcu::FrxProcessorNodeUI<FrxPeakTrackerNode::ProcessorType> >();
	// parameter components
	registerComponentUI<fgc::FrxStdKnob, 
		FrxParameterUI<FrxStdKnob::ControllerType> >();
	// connections
	registerComponentUI<fgc::IOCn, 
		fgcu::FrxConnectionUI<fgc::IOCn::ConnectionType> >();
	registerComponentUI<fgc::ProcessorInputCn, 
		fgcu::FrxConnectionUI<fgc::ProcessorInputCn::ConnectionType> >();
	registerComponentUI<fgc::ProcessorOutputCn, 
		fgcu::FrxConnectionUI<fgc::ProcessorOutputCn::ConnectionType> >();
	registerComponentUI<fgc::ProcessorParameterCn, 
		fgcu::FrxConnectionUI<fgc::ProcessorParameterCn::ConnectionType> >();
	registerComponentUI<fgc::ParameterCn, 
		fgcu::FrxConnectionUI<fgc::ParameterCn::ConnectionType> >();
	registerComponentUI<fgc::ParameterOPCn, 
		fgcu::FrxConnectionUI<fgc::ParameterOPCn::ConnectionType> >();
	// io's
	registerComponentUI<fgc::FrxInputNode, 
		fgcu::FrxIOUI<fgc::FrxInputNode::IOType> >();
	registerComponentUI<fgc::FrxOutputNode, 
		fgcu::FrxIOUI<fgc::FrxOutputNode::IOType> >();
	registerComponentUI<fgc::FrxEntryNode, 
		fgcu::FrxIOUI<fgc::FrxEntryNode::IOType> >();
	registerComponentUI<fgc::FrxExitNode, 
		fgcu::FrxIOUI<fgc::FrxExitNode::IOType> >();
	// misc
	registerComponentUI<fgc::FrxSelection, 
		fgcu::FrxSelectionUI>();
	registerComponentUI<fgc::FrxHover, 
		fgcu::FrxHoverUI>();
	// browser
	using namespace sdc::ui::basic;
	typedef FrxColumnBrowser::BrowserImpl CBrowser;
	registerComponentUI<CBrowser::ColumnViewClass,
		BasicColumnViewUI<CBrowser::ColumnViewClass> >();
	registerComponentUI<CBrowser::ListType, FrxBrowserListUI >();
	registerComponentUI<FrxParameterLabel, FrxParameterLabelUI>();
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
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	static double RADIUS_SMALL = 10.;
	static double RADIUS_MED = 15.;
	static double RADIUS_LARGE = 20.;	
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<global
	m.putProperty("global.background", HtmlColors::getColor("lightblue"));
	m.putProperty("global.foreground", HtmlColors::getColor("white"));
	m.putProperty("ColumnBrowser.fontStyle", createStyle("font-size: 17; font-family: arial"));
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<sizes
	m.putProperty("Knob.mode", std::string("linear"));
	m.putProperty("Knob.size", Dimension(RADIUS_MED*2., RADIUS_MED*2.));
	m.putProperty("Processor.radius", RADIUS_LARGE);
	m.putProperty("ProcessorInput.radius", RADIUS_SMALL);
	m.putProperty("ProcessorOutput.radius", RADIUS_SMALL);
	m.putProperty("Entry.radius", RADIUS_LARGE);
	m.putProperty("Exit.radius", RADIUS_LARGE);
	m.putProperty("StdKnob.radius", RADIUS_SMALL); // affects hit range only
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	m.putProperty("FrxConnection.hitDistance", Coordinate(10.));
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<colors
	m.putProperty("FrxNodeCorona.color",  HtmlColors::getColor("yellow"));
	m.putProperty("ProcessorInput.bgColor", HtmlColors::getColor("white"));
	m.putProperty("ProcessorOutput.bgColor", HtmlColors::getColor("black"));
	m.putProperty("Entry.bgColor", HtmlColors::getColor("black"));
	m.putProperty("Exit.bgColor", HtmlColors::getColor("white"));
	m.putProperty("ProcessorInput.fgColor", HtmlColors::getColor("black"));
	m.putProperty("ProcessorOutput.fgColor", HtmlColors::getColor("black"));
	m.putProperty("Entry.fgColor", HtmlColors::getColor("black"));
	m.putProperty("Exit.fgColor", HtmlColors::getColor("black"));
	m.putProperty("FrxStdKnobCorona02.color",  HtmlColors::getColor("yellow"));
	m.putProperty("FrxStdKnobCorona01.color",  HtmlColors::getColor("red"));
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<styles
	m.putProperty("IOCn.style", 
		createStyle("stroke-width: 4; stroke: grey"));
	m.putProperty("IOCn.hoverStyle", 
		createStyle("stroke-width: 8; stroke: grey"));
	m.putProperty("ProcessorInputCn.style", 
		createStyle("stroke-width: 4; stroke: grey"));
	m.putProperty("ProcessorOutputCn.style", 
		createStyle("stroke-width: 4; stroke: grey"));
	m.putProperty("ProcessorParameterCn.style", 
		createStyle("stroke-width: 2; stroke: red; purple;stroke-dasharray: 9, 5"));
	m.putProperty("ParameterCn.style", 
		createStyle("stroke-width: 4; stroke: green;stroke-dasharray: 9, 5;"));
	m.putProperty("ParameterCn.hoverStyle", 
		createStyle("stroke-width: 8; stroke: green;stroke-dasharray: 9, 5;"));
	m.putProperty("ParameterOPCn.style", 
		createStyle("stroke-width: 4; stroke: grey"));
	m.putProperty("FrxSelection.selectingStyle", 
		createStyle("stroke-width: 4; stroke: grey; fill: purple;stroke-dasharray: 9, 5; fill-opacity: 0.25"));
	m.putProperty("FrxSelection.selectedStyle", 
		createStyle("stroke-width: 4; stroke: grey; fill: purple; fill-opacity: 0.25"));
	m.putProperty("FrxParameterLabel.style", 
		createStyle("stroke-width: 1; stroke: red; fill: grey; fill-opacity: 0.25"));
	m.putProperty("FrxBrowserList.selectedEntryStyle", 
		createStyle("stroke: white; fill: lightblue;"));
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<images
	FileResourceManager &rManager = FileResourceManager::instance();
	rManager.registerImage("FrxCircuidView.image", "disco-floor.png");
	m.putProperty("FrxCircuidView.bgTransfomation", sambag::math::rotate2D(45.));
	m.putProperty("FrxCircuidView.bgExtend", sd::IPattern::DISCO_EXTEND_REPEAT);
	//rManager.registerImage("FrxPluginNode.image", "w3c.svg");
	//rManager.registerImage("ProcessorInput.image", "w3c.svg");
	//rManager.registerImage("ProcessorOutput.image", "w3c.svg");
	rManager.registerImage("Entry.image", "entry.svg");
	rManager.registerImage("Exit.image", "exit.svg");
	rManager.registerImage("FrxBrowser.folder.image", "Blank-icon.png");
	rManager.registerImage("FrxBrowser.default.image", "disco-floor.png");
	rManager.registerImage("FrxBrowser.plugin.image", "vst-big-icon.png");
	rManager.registerImage("FrxBrowser.processor.image", "disco-floor.png");
	rManager.registerImage("FrxBrowser.parameter.image", "knob.png");
}	

}}}} // namespace(s)
