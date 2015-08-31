/*
 * ===========================================================================================================
 * processing.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "processing.h"
#include "com/one4All.h"
#include "parameter/Parameter.h"
#include "OS_Specific/OS_com.h"
#include "OS_Specific/OS_processing.h"
#include "Graph.h"
using namespace com;

namespace processing {
//============================================================================================================
// class ProcessorNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void ProcessorNode::save ( ::com::oArchive &ar, const unsigned int version ) const {
	ar << boost::serialization::base_object<PObject>(*this);
	// ar << parents;  wird in DFSVisitor bzw. ueber updateGraph ermittelt
	// ar << activeChildren; wird in DFSVisitor bzw. ueber updateGraph ermittelt
	size_t numChildFrames = frameContainer.size();
	ar << numChildFrames;
}
//------------------------------------------------------------------------------------------------------------
void ProcessorNode::load ( ::com::iArchive &ar, const unsigned int version ) {
	ar >> boost::serialization::base_object<PObject>(*this);
	// ar >> parents;
	// ar >> activeChildren; wird in DFSVisitor bzw. ueber updateGraph ermittelt
	size_t numChildFrames;
	ar >> numChildFrames;
	prepareFramesContainer( numChildFrames );
}
//------------------------------------------------------------------------------------------------------------
ProcessorNode::ProcessorNode ( const string &name ) : PObject(name), bglVertex(Graph::nullVertex) {
	active = false;
	activeChildren = 0;
	tmpFrames = new Frames();
	delay = 0;
}
//------------------------------------------------------------------------------------------------------------
ProcessorNode::~ProcessorNode() {
	if ( getFrameStackSize()!=0 ) {
		TOLOG ( getName() + " FrameStack Error: " + MyString( getFrameStackSize() ) + " frames left" );
	}
	if (tmpFrames) delete tmpFrames;
}
//------------------------------------------------------------------------------------------------------------
void ProcessorNode::prepareFramesContainer( size_t num ) {
	size_t c = abs( (int)num - (int)frameContainer.size() );
	bool add = num > frameContainer.size();
	while ( c-- > 0 ) {
		if (add) frameContainer.push_back ( Frames::Ptr( new Frames() ) );
		else frameContainer.pop_back();
	}
}
//------------------------------------------------------------------------------------------------------------
Frames * ProcessorNode::mixInputsToFrames( Processor::Int numSamples ) {
	if ( parents.empty() ) {
		tmpFrames->setZero( numSamples );
		return tmpFrames;
	}
	ProcessorNode *n;
	if ( parents.size() == 1 ) { // sonderzug nach pankow:
		n = parents.front();
		assert ( n->isActive() );
		return n->popFrame();
	}
	Parents::iterator it = parents.begin();
	for ( ; it!=parents.end(); ++it ) {
		n = *it;
		if ( !n->isActive() ) continue;
		stream.addFrame ( n->popFrame(), numSamples, getNodeDelay() - n->getNodeDelay() );
	}
	stream.flush ( numSamples, tmpFrames->getData() );
	return tmpFrames;
}
//============================================================================================================
// class ProcessAdapterNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ProcessAdapterNode::ProcessAdapterNode(ProcessAdapter * processAdapter) {
	parent = processAdapter;
}
//------------------------------------------------------------------------------------------------------------
ProcessAdapterNode::~ProcessAdapterNode() {}
//============================================================================================================
// class NOPNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
NOPNode::NOPNode(const string &name) : ProcessorNode ( name ){}
//------------------------------------------------------------------------------------------------------------
void NOPNode::processFrames ( Frames *fr, Processor::Int numSamples ){
	pushAndCopy ( fr, numSamples );
}
//============================================================================================================
// class EndNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void EndNode::processNode( Processor::Int numSamples ) {
	Parents::iterator it = parents.begin();
	ProcessorNode *n;
	for ( ; it!=parents.end(); ++it ) {
		n = *it;
		if ( !n->isActive() ) continue;
		stream.addFrame ( n->popFrame(), numSamples,  getNodeDelay() - n->getNodeDelay() );
	}
}
//============================================================================================================
// class ProcessAdapter::InputNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
const std::string ProcessAdapter::PROPERTY_SWITCH_STATE = "switch state";
//------------------------------------------------------------------------------------------------------------
ProcessAdapter::InputNode::InputNode ( const string &name, ProcessAdapter *parent ) :
	NOPNode (name), 
	parent(parent)
{
}
//------------------------------------------------------------------------------------------------------------
void ProcessAdapter::InputNode::_processNode( Processor::Int numSamples, size_t delay ) { 
	if (delay == 0) {
		ProcessorNode::processNode(numSamples);
		return;
	}
	ProcessorNode *n;
	Parents::iterator it = parents.begin();
	for ( ; it!=parents.end(); ++it ) {
		n = *it;
		if ( !n->isActive() ) continue;
		stream.addFrame ( n->popFrame(), numSamples, delay - n->getNodeDelay() );
	}
	stream.flush ( numSamples, tmpFrames->getData() );
	pushAndCopy(tmpFrames, numSamples);
}
//============================================================================================================
// class ProcessAdapter::OutputNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ProcessAdapter::OutputNode::OutputNode ( const string &name, ProcessAdapter *parent ) :
	NOPNode (name), 
	parent(parent)
{
}
//============================================================================================================
// class ProcessAdapter
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ProcessAdapter::ProcessAdapter( frx::processing::IHostInfo::Ptr  hostInfo, size_t numInputNodes , size_t numOutputNodes ) : 
	hostInfo(hostInfo), 
	inputNodes( InputNodes (numInputNodes) ), 
	outputNodes( OutputNodes(numOutputNodes) )
{
	aNode = ProcessAdapterNode::create ( this ); // zuerst !!
	for ( size_t i=0; i<numInputNodes; ++i ) {
		InputNode::Ptr iNode = InputNode::create( "ProcessAdapter InputNode(" + MyString(i+1) + ")", this );
		inputNodes[i] = iNode;
	}

	for ( size_t i=0; i<numOutputNodes; ++i ) {
		OutputNode::Ptr oNode = OutputNode::create( "ProcessAdapter OutputNode(" + MyString(i+1) + ")", this );
		outputNodes[i] = oNode;
	}
}
//------------------------------------------------------------------------------------------------------------
void ProcessAdapter::setDelay(size_t v) {
	if (inputNodes.size()<=1)
		return;
	for ( size_t i=0; i<inputNodes.size(); ++i ) {
		inputNodes[i]->getDCStream().setMaxDelay(v);
	} 
}
//------------------------------------------------------------------------------------------------------------
ProcessAdapter::InputNode::Ptr ProcessAdapter::createInputNode( const string &name  ) {
	InputNode::Ptr n = InputNode::create ( name, this );
	inputNodes.push_back ( n );
	return n;
}
//------------------------------------------------------------------------------------------------------------
ProcessAdapter::OutputNode::Ptr ProcessAdapter::createOutputNode( const string &name  ) {
	OutputNode::Ptr n = OutputNode::create ( name, this );
	outputNodes.push_back ( n );
	return n;
}
//------------------------------------------------------------------------------------------------------------
void ProcessAdapter::removeInputNode(ProcessAdapter::InputNodePtr node) {
	InputNodes::iterator it = inputNodes.begin();
	for (; it!=inputNodes.end(); ++it) {
		if (*it == node) {
			inputNodes.erase(it);
			break;
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void ProcessAdapter::removeOutputNode(ProcessAdapter::OutputNodePtr node) {
	OutputNodes::iterator it = outputNodes.begin();
	for (; it!=outputNodes.end(); ++it) {
		if (*it == node) {
			outputNodes.erase(it);
			break;
		}
	}
}
//------------------------------------------------------------------------------------------------------------
ProcessAdapter::~ProcessAdapter() {
}
//------------------------------------------------------------------------------------------------------------
size_t ProcessAdapter::getNumActiveOutputNodes() const {
	return aNode->getNumActiveChildren();
}
//------------------------------------------------------------------------------------------------------------
void ProcessAdapter::process(Processor::Int numSamples) {
	if (inputNodes.size() == 1) { // one parent => no dc needed
		inputNodes[0]->_processNode(numSamples, 0);
		processAdapter(numSamples);
		return;
	}
	InputNodes::iterator it = inputNodes.begin();
	for ( ; it!=inputNodes.end(); ++it ) {
		(*it)->_processNode(numSamples, aNode->getNodeDelay());
	}
	// final call
	processAdapter(numSamples);
}
} // namespace processing
