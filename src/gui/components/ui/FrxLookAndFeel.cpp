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
#include <sambag/disco/svg/graphicElements/Style.hpp>
#include <sambag/disco/components/ColumnBrowser.hpp>
#include <sambag/disco/components/List.hpp>
#include <sambag/disco/components/ColumnView.hpp>
#include <sambag/disco/components/ui/basic/BasicColumnViewUI.hpp>
#include <gui/components/FrxComponent.hpp>
#include <gui/components/ui/FrxComponentUI.hpp>
#include <gui/components/FrxConcreteProcessor.hpp>
#include <gui/components/ui/FrxProcessorNodeUI.hpp>
#include <gui/components/ui/FrxSvgProcessorUI.hpp>
#include <gui/components/ui/FrxSvgPluginUI.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/ui/FrxCircuidViewUI.hpp>
#include <gui/components/FrxConcreteConnections.hpp>
#include <gui/components/ui/FrxConnectionUI.hpp>
#include <gui/components/FrxConcreteIO.hpp>
#include <gui/components/ui/FrxIOUI.hpp>
#include <gui/components/ui/FrxSvgIOUI.hpp>
#include <gui/components/ui/FrxSvgKnobUI.hpp>
#include <gui/components/FrxSelection.hpp>
#include <gui/components/FrxPacket.hpp>
#include <gui/components/ui/FrxPacketUI.hpp>
#include <gui/components/FrxHover.hpp>
#include <gui/components/ui/FrxHoverUI.hpp>
#include <gui/components/ui/FrxSelectionUI.hpp>
#include <gui/components/FrxConcreteParameter.hpp>
#include <gui/components/ui/FrxParameterUI.hpp>
#include <gui/components/FrxColumnBrowser.hpp>
#include <gui/components/FrxParameterLabel.hpp>
#include <gui/components/ui/FrxParameterLabelUI.hpp>
#include <gui/components/ui/FrxBrowserListUI.hpp>
#include <gui/components/FrxTooltipManager.hpp>
#include <gui/components/FrxFlag.hpp>
#include <gui/components/ui/FrxFlagUI.hpp>
#include <gui/components/ui/FrxSvgPacketUI.hpp>
#include <sambag/disco/components/Knob.hpp>
#include <gui/components/ui/FrxKnobUI.hpp>
#include <sambag/disco/DiscoHelper.hpp>
#include <sambag/disco/FileResourceManager.hpp>
#include <sambag/disco/IPattern.hpp>
#include <sambag/math/Matrix.hpp>
#include <gui/components/ui/FrxDualUI.hpp>
#include <com/Settings.h>
#include <sambag/disco/svg/graphicElements/ISceneGraph.hpp>

extern const char * globGetLogoPath();

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
//  Class FrxLookAndFeel
//=============================================================================
//-----------------------------------------------------------------------------
FrxLookAndFeel::FrxLookAndFeel() {
	installTooltipManager();
}
//-----------------------------------------------------------------------------
void FrxLookAndFeel::installTooltipManager() {
	if (!sambag::disco::components::getTooltipManagerCreator()) {
		sambag::disco::components::registerTooltipManager(
			&FrxTooltipManager::instance
		);
	}
}
//-----------------------------------------------------------------------------
namespace {
    template <class Processor>
    void registerProcessor(sdcu::ALookAndFeel &laf) {
        namespace fgc = frx::gui::components;
        namespace fgcu = fgc::ui;
        laf.registerComponentUI<Processor,
            fgcu::FrxDualUI<
                fgcu::FrxSvgProcessorUI,
                fgcu::FrxProcessorNodeUI<Processor>
        > >();
    }
    template <>
    void registerProcessor<FrxPluginNode>(sdcu::ALookAndFeel &laf) {
        namespace fgc = frx::gui::components;
        namespace fgcu = fgc::ui;
        laf.registerComponentUI<FrxPluginNode,
            fgcu::FrxDualUI<
                fgcu::FrxSvgPluginUI,
                fgcu::FrxProcessorNodeUI<FrxPluginNode>
        > >();
    }
    template <class Processors>
    void registerProcessors(sdcu::ALookAndFeel &laf) {
        typedef typename Processors::Head FrxProcessor;
        registerProcessor<FrxProcessor>(laf);
        registerProcessors<typename Processors::Tail>(laf);
    }
    template <>
    void registerProcessors<Loki::NullType>(sdcu::ALookAndFeel &laf) {}
}
void FrxLookAndFeel::installComponents() {
	Super::installComponents();
	namespace fgc = frx::gui::components;
	namespace fgcu = fgc::ui;
	
    // view
	registerComponentUI<fgc::FrxCircuidView, fgcu::FrxCircuidViewUI>();
	
    // processor nodes
    registerProcessors<fgc::FrxProcessorList>(*this);
    
    
	// parameter components
	registerComponentUI<fgc::FrxStdKnob,
        fgcu::FrxDualUI<
            FrxSvgKnobUI,
            FrxParameterUI<FrxStdKnob::ControllerType>
    > >();
	registerComponentUI<sdc::Knob, FrxKnobUI<sdc::Knob::Model> >();
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
        fgcu::FrxDualUI<
            fgcu::FrxSvgIOUI,
            fgcu::FrxIOUI<fgc::FrxInputNode::IOType>
    > >();
	registerComponentUI<fgc::FrxOutputNode,
        fgcu::FrxDualUI<
            fgcu::FrxSvgIOUI,
            fgcu::FrxIOUI<fgc::FrxOutputNode::IOType>
    > >();
	registerComponentUI<fgc::FrxEntryNode, 
        fgcu::FrxDualUI<
            fgcu::FrxSvgIOUI,
            fgcu::FrxIOUI<fgc::FrxEntryNode::IOType>
    > >();
	registerComponentUI<fgc::FrxExitNode,
        fgcu::FrxDualUI<
            fgcu::FrxSvgIOUI,
            fgcu::FrxIOUI<fgc::FrxExitNode::IOType>
    > >();
	// misc
	registerComponentUI<fgc::FrxSelection, 
		fgcu::FrxSelectionUI>();
	registerComponentUI<fgc::FrxHover, 
		fgcu::FrxHoverUI>();
	registerComponentUI<fgc::FrxFlag, 
		fgcu::FrxFlagUI>();
    registerComponentUI<fgc::FrxPacket,
		fgcu::FrxDualUI<
            fgcu::FrxSvgPacketUI,
            fgcu::FrxPacketUI
    > >();
	// browser
	using namespace sdc::ui::basic;
	typedef FrxColumnBrowser::BrowserImpl CBrowser;
	registerComponentUI<CBrowser::ColumnViewClass,
		BasicColumnViewUI<CBrowser::ColumnViewClass> >();
	registerComponentUI<CBrowser::ListType, FrxBrowserListUI >();
	registerComponentUI<FrxParameterLabel, FrxParameterLabelUI>();
}
//-------------------------------------------------------------------------
void FrxLookAndFeel::loadStyle(const std::string &svgId,
        const std::string &frxId, const std::string &fallback)
{
    sdcu::UIManager &m = sdcu::getUIManager();
    if (!stylingRef && !loadingStyleRefFailed) {
        stylingRef = sds::Image::create();
        std::string f =
            com::getSettings().getStylePath() + "/frx.gui.default.Misc.svg";
        try {
            stylingRef->setSvgPath(f);
            stylingRef->setSize(sd::Dimension(740, 480));
        } catch(...) {
            SAMBAG_LOG_WARN<<"loading "<<f<<" failed.";
            loadingStyleRefFailed = true;
        }
    }
    if (loadingStyleRefFailed) {
        // set fallback
        m.putProperty(frxId, createStyle(fallback));
        return;
    }
    sdsg::ISceneGraph::Ptr g =
        stylingRef->getSceneGraph();
    sd::IDrawable::Ptr x = g->getElementById(svgId);
    if (!x) {
        // set fallback
        SAMBAG_LOG_WARN<<svgId<<" failed";
        m.putProperty(frxId, createStyle(fallback));
        return;
    }
    sdsg::Style s = g->calculateStyle(x);
    m.putProperty(frxId, s);
}
//-----------------------------------------------------------------------------
void FrxLookAndFeel::loadStyleDefaults() {
    loadStyle("#I2O", "IOCn.style",
              "stroke-width: 4; stroke: darkgrey; stroke-opacity:0.5;");
	loadStyle("#I2O_R", "IOCn.hoverStyle",
		      "stroke-width: 8; stroke: darkgrey; stroke-opacity:0.5;");
	loadStyle("#Pc2I", "ProcessorInputCn.style",
		      "stroke-width: 8; stroke: darkgrey; stroke-opacity:0.5;");
	loadStyle("#Pc2O", "ProcessorOutputCn.style",
              "stroke-width: 8; stroke: darkgrey; stroke-opacity:0.5;");
	loadStyle("#Pc2Pr", "ProcessorParameterCn.style",
		      "stroke-width: 2; stroke: red; purple;stroke-dasharray: 9, 5; stroke-opacity:0.5;");
	loadStyle("#Pr2Pr", "ParameterCn.style",
		      "stroke-width: 4; stroke: green;stroke-dasharray: 9, 5; stroke-opacity:0.5;");
	loadStyle("#Pr2Pr_R", "ParameterCn.hoverStyle",
              "stroke-width: 8; stroke: green;stroke-dasharray: 9, 5; stroke-opacity:0.5;");
	loadStyle("#Prop", "ParameterOPCn.style",
              "stroke-width: 4; stroke: grey;stroke-dasharray: 9, 5; stroke-opacity:0.5;");
	loadStyle("#selecting", "FrxSelection.selectingStyle",
		      "stroke-width: 4; stroke: grey; fill: purple;stroke-dasharray: 9, 5; fill-opacity: 0.25");
	loadStyle("#selected", "FrxSelection.selectedStyle",
              "stroke-width: 4; stroke: black; fill: purple; fill-opacity: 0.25");
	loadStyle("#flag", "FrxFlag.style", 
		      "stroke-width: 1; fill: darkgrey; stroke: darkgrey;font-size: 13; font-family: arial");
	loadStyle("#connecting", "FrxCircuidView.connector", 
		      "stroke-width: 2; stroke: red;");
}
//-----------------------------------------------------------------------------
void FrxLookAndFeel::reloadStyleDefaults() {
    loadStyleDefaults();
}
//-----------------------------------------------------------------------------
void FrxLookAndFeel::installDefaults() {
	Super::installDefaults();
	using namespace sambag::disco;
	using namespace sambag::disco::svg;
	using namespace sambag::disco::svg::graphicElements;
	using namespace sambag::disco::components;
	using namespace sambag::disco::components::ui;
	using namespace sambag::disco::components::ui::basic;
	UIManager &m = getUIManager();
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<install twice guard
	bool alreadyInstalled = false;
	m.getProperty("FrxLookAndFeel.installed", alreadyInstalled);
	if (alreadyInstalled) {
		return;
	}
	alreadyInstalled = true;
	m.putProperty("FrxLookAndFeel.installed", alreadyInstalled);
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	static double RADIUS_SMALL = 10.;
	static double RADIUS_MED = 15.;
	static double RADIUS_LARGE = 20.;	
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<global
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
	m.putProperty("Knob.wheelIncrement", (double)0.01);
	m.putProperty("ProcessorIO.stateActiveRadius",  RADIUS_SMALL * 1.7);
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	m.putProperty("FrxConnection.hitDistance", Coordinate(10.));
	m.putProperty("FrxNodeCorona.fadeAnimation.duration", (long)150);
	m.putProperty("FrxNodeCorona.fadeAnimation.refreshRate", (long)15);
	m.putProperty("FrxNodeCorona.fadeAnimation.tweenType", std::string("lin"));
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<colors
	m.putProperty("FrxNodeCorona.color",  HtmlColors::getColor("yellow"));
	m.putProperty("ProcessorInput.bgColor", HtmlColors::getColor("white"));
	m.putProperty("ProcessorOutput.bgColor", HtmlColors::getColor("black"));
	m.putProperty("Entry.bgColor", HtmlColors::getColor("black"));
	m.putProperty("Exit.bgColor", HtmlColors::getColor("white"));
	m.putProperty("ProcessorInput.fgColor", sd::ColorRGBA(.352, .76, 1.));
	m.putProperty("ProcessorOutput.fgColor", sd::ColorRGBA(.352, .76, 1.));
	m.putProperty("ProcessorInput.bgColor", sd::ColorRGBA(0, 0, 0, 0));
	m.putProperty("ProcessorOutput.bgColor", sd::ColorRGBA(.352, .76, 1.));
	m.putProperty("Entry.fgColor", HtmlColors::getColor("black"));
	m.putProperty("Exit.fgColor", HtmlColors::getColor("black"));
	m.putProperty("FrxStdKnobCorona02.color",  HtmlColors::getColor("yellow"));
	m.putProperty("FrxStdKnobCorona01.color",  HtmlColors::getColor("red"));
	m.putProperty("ProcessorIO.stateActiveColor",  HtmlColors::getColor("red"));
	m.putProperty("FrxCircuidView.bgColor",  HtmlColors::getColor("orange"));
	m.putProperty("Knob.strokeColor", sd::ColorRGBA(.352, .76, 1.));
	m.putProperty("Knob.fillColor", sd::ColorRGBA(0,0,0,0));
	m.putProperty("Knob.colorHandler", sd::ColorRGBA(.352, .76, 1.));
	typedef sd::IGradient::ColorStop Stop;
	sd::IGradient::ColorStops stops(4);
	stops[0] = Stop( sd::ColorRGBA(0.843137254902, 0.360784313725, 0.560784313725, .7), 0);
	stops[1] = Stop( sd::ColorRGBA(0.564705882353, 0.0666666666667, 0.317647058824, .7), 0.04);
	stops[2] = Stop( sd::ColorRGBA(0.36862745098, 0.0, 0.176470588235, .7), 0.42);
	stops[3] = Stop( sd::ColorRGBA(0.152941176471, 0.0078431372549, 0.0941176470588, .7), 1.0);
	m.putProperty("FrxCircuidView.bg.gradient.colorStops", stops);
    m.putProperty("FrxCircuidView.bg.parallaxEffect", 1.5);
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<styles
	Style style = createStyle("stroke:darkgrey; fill:lightgrey; font-size: 12; font-family: arial; font-style: italic;");
	//style.fillPattern( sd::createPattern("lin(0,1,[D3D3D3FF:0, D3D3D3FF:0.66, D3D3D3FF:0.667, FF:1])") );
	m.putProperty("SetupWindow.style", style);
	m.putProperty("FrxComponent.menu.label.style", 
		createStyle("stroke:darkgrey; fill:royalblue; font-size: 12; font-family: arial; font-style: italic;"));
	m.putProperty("FrxParameterLabel.style", 
		createStyle("stroke-width: 1; stroke: red; fill: grey; fill-opacity: 0.25"));
	m.putProperty("FrxBrowserList.selectedEntryStyle", 
		createStyle("stroke: white; fill: lightblue;"));
	m.putProperty("StatusMessage.style", 
		createStyle("stroke-width: 1; stroke: darkgrey;font-size: 13; font-family: arial"));
	m.putProperty("ProcessorInput.displayStyle", createStyle("fill: white; font-size: 10; font-family: arial"));
	m.putProperty("ProcessorOutput.displayStyle", createStyle("fill: white; font-size: 10; font-family: arial"));
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<style defaults
    loadStyleDefaults();
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<images
	FileResourceManager *rManager = dynamic_cast<FileResourceManager*> (
		&getResourceManager()
	);
	rManager->registerImage("FrxCircuidView.image", "images/bkgrey.png");
	m.putProperty("FrxCircuidView.bgTransfomation", sambag::math::scale2D(.25, .25));
	m.putProperty("FrxCircuidView.bgExtend", sd::IPattern::DISCO_EXTEND_REPEAT);
	m.putProperty("FrxCircuidView.bgOpacity", (double)0.02);
	rManager->registerImage("StatusMessage.icon.default", "images/inf_icon.png");
	rManager->registerImage("StatusMessage.icon.hint", "images/inf_icon.png");
	rManager->registerImage("StatusMessage.icon.warning", "images/warning_icon.png");
	rManager->registerImage("Plugin.image", "images/vstPlugNode.png");
	rManager->registerImage("Plugin.e.image", "images/e.png");
	rManager->registerImage("Volume.image", "images/volume_node.png");
	rManager->registerImage("Pan.image", "images/channel_split.png");
	rManager->registerImage("InputStep.image", "images/step_adapter.png");
	rManager->registerImage("OutputStep.image", "images/step_adapter.png");
	rManager->registerImage("OutputSwitch.image", "images/oSwitch.png");
	rManager->registerImage("InputSwitch.image", "images/oSwitch.png");
	rManager->registerImage("ADSRTrigger.image", "images/adsr_trigger.png");
	rManager->registerImage("PeakTracker.image", "images/peak_tracker.png");
	rManager->registerImage("MidiProcessor.image", "images/midiReceiver.png");
    rManager->registerImage("RemoteChReceiver.image", "images/remoteChReceiver.png");
	
	//rManager->registerImage("ProcessorInput.image", "w3c.svg");
	//rManager->registerImage("ProcessorOutput.image", "w3c.svg");
	rManager->registerImage("Entry.image", "images/entry.png");
	rManager->registerImage("Exit.image", "images/exit.png");
	rManager->registerImage("FrxBrowser.folder.image", "images/folder.png");
    rManager->registerImage("FrxBrowser.historyFolder.image", "images/hfolder.png");
	rManager->registerImage("FrxBrowser.default.image", "images/disco-floor.png");
	rManager->registerImage("FrxBrowser.plugin.image", "images/plugin.png");
	rManager->registerImage("FrxBrowser.processor.image", "images/processor.png");
	rManager->registerImage("FrxBrowser.preset.image", "images/processor.png");
	rManager->registerImage("FrxBrowser.parameter.image", "images/knob.png");
	rManager->registerImage("FrxBrowser.pluginInstrument.image", "images/iplug.png");
	rManager->registerImage("FrxBrowser.addContent.image", "images/plus.png");
	rManager->registerImage("About.logo", "images/logo.png");
	rManager->registerImage("About.bg", "images/disco-floor.png");
    rManager->registerImage("FrxPacket.image", "images/packet.png");
    
    std::string logoPath( globGetLogoPath() );
    if (!logoPath.empty()) {
        rManager->registerImage("FrxCircuidView.logo", logoPath);
    }
    
    
	resetUIPorpertyCache();
}	

}}}} // namespace(s)
