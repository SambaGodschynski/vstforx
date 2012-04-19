/*
 * ===========================================================================================================
 * ViewCommand.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "ViewCommand.h"
#include "ConcreteGObjects.h"
#include "CircuidView.h"
#include "CircuidControl.h"
#include "processing/Plugin.h"
#include "processing/ConcreteProcessAdapter.h"
#include "processing/parameter/parameter.h"
#include "processing/parameter/ConnectionOperators.h"
#include "GObjectController.h"
#include "com/PluginCollection.h"
#include "processing/pluginTypes/VSTPlugin2x.h"
#include <string>


namespace ppiGui{
//============================================================================================================
//	Klasse CmdConnectGKnob:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdConnectGKnob::_execute(){
	// Modell
	Parameter::Ptr pA = ctrl->getViewRelations().get<Parameter> ( src );
	Parameter::Ptr pB = ctrl->getViewRelations().get<Parameter> ( dst );
	Graph::Ptr g = getRelatedGraph(cView);
	if (!g)
		throw com::ppiError::NullPointer("NULL Pointer", __FILE__, __LINE__);
	if (!g->connectParameter(pA, pB))
		return;
	pB->setValue ( *pA );
	// View
	gc = GConnectionPaPa::create ( cView, src, dst );
	ctrl->registerObject ( gc );
	cView->addGObject ( gc, CircuidView::CONNECTIONS );
	cView->CView::setDirty();
	TOLOG ( pA->getName() + " connected to " + pB->getName() );
}
//------------------------------------------------------------------------------------------------------------
Command::Ptr CmdConnectGKnob::createReverseCommand (){
	if (!gc) throw ppiError::NullPointer ("NullPointer", __FILE__, __LINE__ );
	return Command::Ptr ( new CmdRemoveGObject ( cView, gc ) );
}
//============================================================================================================
//	Klasse CmdConnectIONode:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdConnectGIONode::_execute(){
	// gInputs / gOutput zuordnen
	GInputNode *input; GOutputNode *output; 
	relate< GInputNode, GOutputNode, GObject > ( &input, &output, src.get(), dst.get() );
	if ( !input || !output ) return;
	
	// hole zu ViewObjects zugehoerige PObjects
	GObject::Ptr pInput = input->getPtr(); 
	GObject::Ptr pOutput = output->getPtr();
	ProcessorNode::Ptr in = ctrl->getViewRelations().get<ProcessorNode> ( pInput );
	ProcessorNode::Ptr out = ctrl->getViewRelations().get<ProcessorNode> ( pOutput );
	// verbindung im graph vollziehen
	Graph::Ptr g = getRelatedGraph( src->getParentView() );
	Graph::Janitor::Ptr janitor = g->getJanitor();
	Graph::Janitor::State ret = janitor->connectNodes( out.get(), in.get() );
	if ( ret == Graph::Janitor::FAILED ) return; // verbindung nicht valid
	
	// verbindung in view vollziehen
	GConnectionIO::Ptr gc = GConnectionIO::create ( cView, pInput, pOutput );
	ctrl->registerObject ( gc ); // neue verbindung im controller reg.
	cView->addGObject ( gc, CircuidView::CONNECTIONS );

	TOLOG ( out->getName() + " connect to " + in->getName() );
}
//============================================================================================================
//	Klasse CmdAddVSTPlugNode:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdAddVSTPlugNodeMan::_execute(){

	// datei namen auswaehlen
	VstFileType vstPlugType ("VST Plugin File", 0, "dll");
	string filename = selectFile( &vstPlugType, "Select VST-Plugin:", "" );
	if (filename.length()==0) return;
	
	Graph::Ptr graph = getRelatedGraph ( cView );

	// erstelle VSTPlugin
	Plugin::Ptr adapter;
	try {
		adapter = PluginFactory::createPlugNode( graph.get(), filename );
	} catch (...){
		cView->CView::setDirty();
		return;
	}
	if ( !adapter ) return;
	
	// adapter im graph einfuegen ( danach janitor freigeben um graph::processingLock freizugeben
	Graph::Janitor::Ptr updater = graph->getJanitor(); // <-------------------------Graph::processingLock-Start
	updater->add(adapter);
	updater.reset();  // <----------------------------------------------------------Graph::processingLock-Ende
	
	// weiter im text...
	int numINodes = adapter->getNumInputNodes();
	int numONodes = adapter->getNumOutputNodes();

	// erzeuge view objs
	CPoint point; cView->getMouseLocation (point);
	
	// GObjekt erzeugen 
	GVSTPlugNode::Ptr gProcessor = GVSTPlugNode::create ( cView );
	// GObject registrieren
	ctrl->registerObject ( gProcessor, adapter );
	// an Mauspos. verschieben
	gProcessor->moveTo ( point );
	// GIONodes Erzeugen
	gProcessor->createIONodes( numINodes, numONodes );
	const GProcessorNode::InputNodeContainer  &ins =  gProcessor->getInputNodes();
	const GProcessorNode::OutputNodeContainer &outs = gProcessor->getOutputNodes();
	for ( int i=0; i<numINodes; ++i ) {
		// GObject: Eingangsknoten
		GObject::Ptr in = ins.at(i);
		// registriere GObject
		ctrl->registerObject ( in, adapter->getInputNode(i) );
		in->setName ( gProcessor->getName() + "::input" );
		GConnection::Ptr con = GConnectionPrIn::create ( cView, gProcessor, in );
		cView->addGObject ( con, CircuidView::CONNECTIONS );
		ctrl->registerObject ( con );
	}
	for ( int i=0; i<numONodes; i++ ) {
		// Ausgangsknoten gProcessor
		GObject::Ptr out = outs.at(i);
		// registriere GObject
		ctrl->registerObject ( out, adapter->getOutputNode(i) );
		out->setName ( gProcessor->getName() + "::output" + MyString(i+1) );
		// Connection:
		GConnection::Ptr con = GConnectionPrOut::create ( cView, gProcessor, out );
		cView->addGObject ( con, CircuidView::CONNECTIONS );
		ctrl->registerObject ( con );
	}
	GObjectList gObjs;
	gObjs.push_back( gProcessor );
	gProcessor->getIOs ( gObjs );
	PlaceGObject::Ptr pG = PlaceGObject::create ( cView, gObjs );
	cView->addGObject ( pG );
	cView->CView::setDirty();
}
//------------------------------------------------------------------------------------------------------------
void CmdAddVSTPlugNode::_execute(){

	PluginCollection::Ptr pC = PluginCollection::getPluginCollection();
	Graph::Ptr graph = getRelatedGraph ( cView );
	// get plugin from database
	Plugin::Ptr adapter;
	CPoint point; cView->getMouseLocation (point);

	try {
		adapter = pC->restorePlugNode( graph.get(), plugInfo ); 
	} catch (const VSTPlugin::ShellPluginException &ex) {
		CircuidControl::Ptr ctrl = getCircuidControl(cView);
		if (!ctrl) {
			return;
		}
		ctrl->showVSTShellSelectionMenu(point, plugInfo, ex.content);
		return;
	}
	// adapter im graph einfuegen ( danach janitor freigeben um graph::processingLock freizugeben )
	Graph::Janitor::Ptr updater = graph->getJanitor(); // <-------------------------Graph::processingLock-Start
	updater->add(adapter);
	updater.reset();  // <----------------------------------------------------------Graph::processingLock-Ende
	// erzeuge view objs
	int numINodes = adapter->getNumInputNodes();
	int numONodes = adapter->getNumOutputNodes();
	
	// GObjekt erzeugen 
	GVSTPlugNode::Ptr gProcessor = GVSTPlugNode::create ( cView );
	// GObject registrieren
	ctrl->registerObject ( gProcessor, adapter );
	// an Mauspos. verschieben
	gProcessor->moveTo ( point );
	// GIONodes Erzeugen
	gProcessor->createIONodes( numINodes, numONodes );
	const GProcessorNode::InputNodeContainer  &ins =  gProcessor->getInputNodes();
	const GProcessorNode::OutputNodeContainer &outs = gProcessor->getOutputNodes();
	for ( int i=0; i<numINodes; ++i ) {
		// GObject: Eingangsknoten
		GObject::Ptr in = ins.at(i);
		// registriere GObject
		ctrl->registerObject ( in, adapter->getInputNode(i) );
		in->setName ( gProcessor->getName() + "::input" );
		GConnection::Ptr con = GConnectionPrIn::create ( cView, gProcessor, in );
		cView->addGObject ( con, CircuidView::CONNECTIONS );
		ctrl->registerObject ( con );
	}
	for ( int i=0; i<numONodes; i++ ) {
		// Ausgangsknoten gProcessor
		GObject::Ptr out = outs.at(i);
		// registriere GObject
		ctrl->registerObject ( out, adapter->getOutputNode(i) );
		out->setName ( gProcessor->getName() + "::output" + MyString(i+1) );
		// Connection:
		GConnection::Ptr con = GConnectionPrOut::create ( cView, gProcessor, out );
		cView->addGObject ( con, CircuidView::CONNECTIONS );
		ctrl->registerObject ( con );
	}
	GObjectList gObjs;
	gObjs.push_back( gProcessor );
	gProcessor->getIOs ( gObjs );
	PlaceGObject::Ptr pG = PlaceGObject::create ( cView, gObjs );
	cView->addGObject ( pG );
	cView->CView::setDirty();
}
//============================================================================================================
//	Klasse CmdCreateProcessorNode:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename GPrNode, typename Adapter, int numINodes, int numONodes >
void CmdCreateProcessorNode::create(){
	Graph::Ptr graph = getRelatedGraph ( cView );
	CPoint point; cView->getMouseLocation (point);
	// Graph Objekt erzeugen 
	typename Adapter::Ptr adapter = Adapter::create( graph.get() );
	// adapter im graph einfuegen
	Graph::Janitor::Ptr updater = graph->getJanitor(); // <-------------------------Graph::processingLock-Start
	updater->add(adapter);
	updater.reset();  // <----------------------------------------------------------Graph::processingLock-Ende
	// GObjekt erzeugen 
	typename GPrNode::Ptr gProcessor = GPrNode::create ( cView );
	// GObject registrieren
	ctrl->registerObject ( gProcessor, adapter );
	// an Mauspos. verschieben
	gProcessor->moveTo ( point );
	// GIONodes Erzeugen
	gProcessor->createIONodes( numINodes, numONodes );
	const GProcessorNode::InputNodeContainer  &ins =  gProcessor->getInputNodes();
	const GProcessorNode::OutputNodeContainer &outs = gProcessor->getOutputNodes();
	for ( int i=0; i<numINodes; ++i ) {
		// GObject: Eingangsknoten
		GObject::Ptr in = ins.at(i);
		// registriere GObject
		ctrl->registerObject ( in, adapter->getInputNode(i) );
		in->setName ( gProcessor->getName() + "::input" );
		// Connection:
		GConnection::Ptr con = GConnectionPrIn::create ( cView, gProcessor, in );
		cView->addGObject ( con, CircuidView::CONNECTIONS );
		ctrl->registerObject ( con );
	}
	for ( int i=0; i<numONodes; i++ ) {
		// Ausgangsknoten gProcessor
		GObject::Ptr out = outs.at(i);
		// registriere GObject
		ctrl->registerObject ( out, adapter->getOutputNode(i) );
		out->setName ( gProcessor->getName() + "::output" + MyString(i+1) );
		// Connection:
		GConnection::Ptr con = GConnectionPrOut::create ( cView, gProcessor, out );
		cView->addGObject ( con, CircuidView::CONNECTIONS );
		ctrl->registerObject ( con );
	}
	newGPr = gProcessor;
	newPrA = adapter;
}
//============================================================================================================
//	Klasse CmdCreateVolumeNode:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdCreateVolumeNode::_execute(){
	GObjectList gObjs;
	create<GVolumeNode, Volume, 1, 1>();
	// erzeugten Volume holen
	Volume::Ptr vol = boost::shared_dynamic_cast<Volume, PObject>(newPrA);
	// knob erzeugen
	GKnob::Ptr knob = GStdKnob::create ( cView );
	// knob an erzeugte GVolumeNode pos. verschieben
	CPoint p (newGPr->getPos());
	p.offset ( newGPr->getSize().width(), -newGPr->getSize().height() ); 
	knob->moveTo ( p );
	//knob der view hinzufuegen
	gObjs.push_back( knob );
	// Knob Connection
	gObjs.push_back ( newGPr );
	GConnection::Ptr gc = GConnectionPrPa::create ( cView, newGPr, knob );
	ctrl->registerObject ( gc );
	//Knob Connection der view hinzufuegen
	cView->addGObject ( gc, CircuidView::CONNECTIONS);
	// volume Knob registrieren und init.
	Parameter::Ptr param = vol->getParameter();
	ctrl->registerObject ( knob, param );
	param->setValue (1.0);
	knob->setHeader ( param->getName() );
	knob->setDisplay( param->getDisplay() );
	newGPr->getIOs ( gObjs );
	PlaceGObject::Ptr pG = PlaceGObject::create ( cView, gObjs );
	cView->addGObject ( pG );
	cView->CView::setDirty();
}
//============================================================================================================
//	Klasse CmdCreatePanAdapter:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdCreatePanAdapter::_execute(){
	GObjectList gObjs;
	create<GPanAdapter, Pan, 1, 1>();
	gObjs.push_back( newGPr );
	newGPr->getIOs ( gObjs );
	PlaceGObject::Ptr pG = PlaceGObject::create ( cView, gObjs );
	cView->addGObject ( pG );
	cView->CView::setDirty();
}
//============================================================================================================
//	Klasse CmdCreateOStepNode:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdCreateOStepNode::_execute(){
	GObjectList gObjs;
	create<GOutputStepNode, OutputStep, 1, 2>();
	gObjs.push_back( newGPr );
	newGPr->getIOs ( gObjs );
	PlaceGObject::Ptr pG = PlaceGObject::create ( cView, gObjs );   
	cView->addGObject ( pG );
	cView->CView::setDirty();
}
//============================================================================================================
//	Klasse CmdCreateIStepNode:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdCreateIStepNode::_execute(){
	GObjectList gObjs;
	create<GInputStepNode, InputStep, 2, 1>();
	gObjs.push_back( newGPr );
	newGPr->getIOs ( gObjs );
	PlaceGObject::Ptr pG = PlaceGObject::create ( cView, gObjs );
	cView->addGObject ( pG );
	cView->CView::setDirty();
}
//============================================================================================================
//	Klasse CmdCreateOutputSwitchNode:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdCreateOutputSwitchNode::_execute(){
	GObjectList gObjs;
	create<GOutputSwitch, OutputSwitch, 1, 2>();
	gObjs.push_back( newGPr );
	newGPr->getIOs ( gObjs );
	PlaceGObject::Ptr pG = PlaceGObject::create ( cView, gObjs );
	cView->addGObject ( pG );
	cView->CView::setDirty();
}
//============================================================================================================
//	Klasse CmdCreateInputSwitchNode:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdCreateInputSwitchNode::_execute(){
	GObjectList gObjs;
	create<GInputSwitch, InputSwitch, 2, 1>();
	gObjs.push_back( newGPr );
	newGPr->getIOs ( gObjs );
	PlaceGObject::Ptr pG = PlaceGObject::create ( cView, gObjs );
	cView->addGObject ( pG );
	cView->CView::setDirty();
}
//============================================================================================================
//	Klasse CmdCreatePeakTracker:
//  Erstellt Volume2ValueNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdCreatePeakTracker::_execute(){
	GObjectList gObjs;
	create<GPeakTracker, PeakTracker, 1, 0>();
	// downcast created processorNode
	PeakTracker::Ptr vol = boost::shared_dynamic_cast<PeakTracker, PObject>(newPrA);
	// unsichtbare verbindung:
	Graph::Ptr graph = getRelatedGraph ( cView );
	Graph::Janitor::Ptr janitor = graph->getJanitor();
	janitor->connectNodes( newPrA->getOutputNode(0).get(), graph->getEndNode().get() );
	// Output Knob
	GKnob::Ptr knob = GPassiveKnob::create ( cView );
	ctrl->registerObject ( knob, vol->getOutParameter() );
	// knob flag:
	knob->setHeader ( vol->getOutParameter()->getName() );
	knob->setDisplay ( vol->getOutParameter()->getDisplay() );
	// knob pos.
	CPoint p (newGPr->getPos());
	p.offset ( newGPr->getSize().width(), -newGPr->getSize().height() ); 
	knob->moveTo ( p );
	gObjs.push_back( knob );
	gObjs.push_back( newGPr );
	// Knob Connection
	GConnection::Ptr gc = GConnectionPrPa::create ( cView, newGPr, knob );
	ctrl->registerObject ( gc );
	cView->addGObject ( gc, CircuidView::CONNECTIONS);
	newGPr->getIOs ( gObjs );
	PlaceGObject::Ptr pG = PlaceGObject::create ( cView, gObjs );
	cView->addGObject ( pG );
	cView->CView::setDirty();
}
//============================================================================================================
//	Klasse CmdCreateADSRTriggerNode:
//  Erstellt AdsrTriggerNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdCreateADSRTriggerNode::_execute(){
	GObjectList gObjs;
	create<GADSRTrigger, ADSRTrigger, 1, 0>();
	// downcast created processorNode
	ADSRTrigger::Ptr adsr = boost::shared_dynamic_cast<ADSRTrigger, PObject>(newPrA);
	// unsichtbare verbindung:
	Graph::Ptr graph = getRelatedGraph ( cView );
	Graph::Janitor::Ptr janitor = graph->getJanitor();
	janitor->connectNodes( newPrA->getOutputNode(0).get(), graph->getEndNode().get() );
	// Outp. knob
	GKnob::Ptr knob = GPassiveKnob::create ( cView );
	ctrl->registerObject ( knob, adsr->getOutParameter() );
	// knob flag:
	knob->setHeader ( adsr->getOutParameter()->getName() );
	knob->setDisplay ( adsr->getOutParameter()->getDisplay() );
	// knob pos.
	CPoint p (newGPr->getPos());
	p.offset ( newGPr->getSize().width(), -newGPr->getSize().height() ); 
	knob->moveTo ( p );
	gObjs.push_back( knob );
	gObjs.push_back( newGPr );
	// Knob Connection
	GConnection::Ptr gc = GConnectionPrPa::create ( cView, newGPr, knob );
	ctrl->registerObject ( gc );
	cView->addGObject ( gc, CircuidView::CONNECTIONS);
	newGPr->getIOs ( gObjs );
	PlaceGObject::Ptr pG = PlaceGObject::create ( cView, gObjs );
	cView->addGObject ( pG );
	cView->CView::setDirty();
}
//============================================================================================================
//	Klasse CmdCreateMidiProcessor:
//  Erstellt CmdCreateMidiProcessor
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdCreateMidiProcessor::_execute(){
	GObjectList gObjs;
	create<GMidiProcessor, MidiProcessor, 0, 0>();
	gObjs.push_back( newGPr );
	PlaceGObject::Ptr pG = PlaceGObject::create ( cView, gObjs );
	cView->addGObject ( pG );
	cView->CView::setDirty();
}
//============================================================================================================
//	Klasse CmdCreateFreeGKnob
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdCreateFreeGKnob::_execute(){
	Graph::Ptr graph = getRelatedGraph ( cView );
	CPoint point;
	cView->getMouseLocation (point);
	Parameter::Ptr p = Parameter::create();
	p->setName ( "free knob" );
	GKnob::Ptr knob = GStdKnob::create ( cView );
	ctrl->registerObject ( knob, p );
	knob->moveTo ( point );
	knob->setHeader ( p->getName() );
	knob->setDisplay( p->getDisplay() );
	PlaceGObject::Ptr pG = PlaceGObject::create ( cView, knob );
	cView->addGObject ( pG );
	// add to graph
	graph->getJanitor()->add ( p );
	cView->CView::setDirty();
}
//============================================================================================================
//	Klasse CmdRemoveGObject
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdRemoveGObject::_execute() { 
	cView->removeGObject ( toRemove );
	cView->CView::setDirty();
}
//============================================================================================================
//	Klasse CmdCreateHostGKnob
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdCreateHostGKnob::_execute(){
	Graph::Ptr graph = getRelatedGraph ( cView );
	CPoint point;
	cView->getMouseLocation (point);
	Parameter::Ptr p = graph->getHostParameter (index);
	GKnob::Ptr knob = GStdKnob::create ( cView );
	ctrl->registerObject ( knob, p );
	knob->moveTo ( point );
	knob->setHeader ( p->getName() );
	knob->setDisplay( p->getDisplay() );
	PlaceGObject::Ptr pG = PlaceGObject::create ( cView, knob );
	cView->addGObject ( pG );
	cView->CView::setDirty();
	TOLOG ( "HostKnob:" + p->getName() + " added." );
}

//============================================================================================================
//	Klasse CmdPlaceGKnob
//  Erstellt PlaceGObject Objekt und fuegt es view hinzu.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdPlaceGKnob::_execute() {
	CPoint point;
	cView->getMouseLocation (point);
	GKnob::Ptr knob = GStdKnob::create ( cView );
	knob->setHeader ( param->getName() );
	knob->setDisplay ( param->getDisplay() );
	knob->setValue ( *param );
	ctrl->registerObject ( knob, param );
	knob->moveTo (point);
	GConnection::Ptr gc = GConnectionPrPa::create ( cView, knob, gObj );
	ctrl->registerObject ( gc );
	cView->addGObject ( gc, CircuidView::CONNECTIONS );
	PlaceGObject::Ptr pC = PlaceGObject::create( cView, knob );
	cView->addGObject ( pC );
	cView->setDirty(knob.get());
	TOLOG ( gObj->getName() + "->" + param->getName() + " added." );
}
//============================================================================================================
//	Klasse CmdAddOutput:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdAddOutput::_execute(){
	// OutputStep holen:
	ProcessAdapter::Ptr adapter = ctrl->getViewRelations().get<ProcessAdapter> (vObj);
	VariableOutputAdapter *sa = dynamic_cast<VariableOutputAdapter*> ( adapter.get() );
	if ( !sa ) throw com::ppiError::TypeError( "TypeError", __FILE__, __LINE__ );

	ProcessAdapter::OutputNode::Ptr newNode = 
		boost::shared_dynamic_cast<ProcessAdapter::OutputNode, PObject> ( sa->addOutputNode() );

	if (!newNode) throw com::ppiError::SizeError ("no output created", __FILE__, __LINE__ );
	
	// im graph einfuegen 
	Graph::Ptr graph = getRelatedGraph ( cView );
	Graph::Janitor::Ptr updater = graph->getJanitor(); // <-------------------------Graph::processingLock-Start
	updater->add(newNode);
	updater.reset();  // <----------------------------------------------------------Graph::processingLock-Ende

	// gOutput node erstellen...
	CPoint point;
	cView->getMouseLocation (point);
	GOutputNode::Ptr newGNode = GOutputNode::create ( cView, Resources::VSTPLUG_OUTPUT );
	//gobject registrieren
	ctrl->registerObject ( newGNode, newNode );
	newGNode->moveTo (point);
	newGNode->setName ("Step node GOutput");
	vObj->outs.push_back ( newGNode ); //GProcessorNode::OutputNodeContainer
	// ...connection...
	GConnection::Ptr gc = GConnectionPrOut::create ( cView, newGNode,  vObj );
	cView->addGObject ( gc, CircuidView::CONNECTIONS);
	// ... und platzieren
	PlaceGObject::Ptr pC = PlaceGObject::create( cView, newGNode );
	cView->addGObject ( pC );
	cView->setDirty(newGNode.get());
	PObject *p = dynamic_cast<PObject*> ( sa );
	if (p) TOLOG ( p->getName() + " outputNode added." );
}
//============================================================================================================
//	Klasse CmdAddInput:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdAddInput::_execute(){
	// InputStep holen:
	ProcessAdapter::Ptr adapter = ctrl->getViewRelations().get<ProcessAdapter> (vObj);
	VariableInputAdapter *sa = dynamic_cast<VariableInputAdapter*> ( adapter.get() );
	if ( !sa ) throw com::ppiError::TypeError( "TypeError", __FILE__, __LINE__ );
	
	ProcessAdapter::InputNode::Ptr newNode = 
		boost::shared_dynamic_cast<ProcessAdapter::InputNode, PObject> ( sa->addInputNode() );
	
	if (!newNode) throw com::ppiError::SizeError ("no input created", __FILE__, __LINE__ );
	
	// im graph einfuegen 
	Graph::Ptr graph = getRelatedGraph ( cView );
	Graph::Janitor::Ptr updater = graph->getJanitor(); // <-------------------------Graph::processingLock-Start
	updater->add(newNode);
	updater.reset();  // <----------------------------------------------------------Graph::processingLock-Ende

	// gInput node erstellen...
	CPoint point;
	cView->getMouseLocation (point);
	GInputNode::Ptr newGNode = GInputNode::create ( cView, Resources::VSTPLUG_INPUT );
	//gobject registrieren
	ctrl->registerObject ( newGNode, newNode );
	newGNode->moveTo (point);
	newGNode->setName ("Step node GInput");
	vObj->ins.push_back ( newGNode ); //GProcessorNode::OutputNodeContainer
	// ...connection...
	GConnection::Ptr gc = GConnectionPrIn::create ( cView, newGNode,  vObj );
	cView->addGObject ( gc, CircuidView::CONNECTIONS);
	// ... und platzieren
	PlaceGObject::Ptr pC = PlaceGObject::create( cView, newGNode );
	cView->addGObject ( pC );
	cView->setDirty(newGNode.get());
	PObject *p = dynamic_cast<PObject*> ( sa );
	if (p) TOLOG ( p->getName() + " inputNode added." );
}
//============================================================================================================
//	Klasse CmdChangeParameter
//  Aendert den wert eines Parameter
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdChangeParameter::_execute(){
	parameter->setValue ( value );
}
//============================================================================================================
//	Klasse CmdMove:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdMove::_execute(){
	obj->moveTo(pos);
}
//============================================================================================================
//	Klasse CmdRemoveMenu
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdRemoveMenu::_execute() {
	_menu->getFrame()->setFocusView ( _menu.get() );	
	_menu->getFrame()->removeView ( _menu.get(), false );
	_menu->getFrame()->setDirty();
}
//============================================================================================================
//	Klasse CmdSetProgram 
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdSetProgram::_execute() {
	Plugin::Ptr plug = ctrl->getViewRelations().get<Plugin> ( obj );
	if (!plug) return;
	plug->setProgram( programIndex );
}
//============================================================================================================
//	Klasse CmdOpenEditor 
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdOpenEditor::_execute() {
	view->openWindow( pos.x, pos.y );
}
//============================================================================================================
//	Klasse CmdCloseEditor 
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdCloseEditor::_execute() {
	view->closeWindow();
}
}// namespace ppiGui



