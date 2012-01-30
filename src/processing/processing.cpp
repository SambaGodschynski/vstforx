
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
ProcessorNode::ProcessorNode ( const string &name ) : PObject(name), bglVertex(Graph::nullVertex) {
	active = false;
	activeChildren = 0;
	tmpFrame = new Frame();
	delay = 0;
}
//------------------------------------------------------------------------------------------------------------
ProcessorNode::~ProcessorNode() {
	if ( getFrameStackSize()!=0 ) {
		TOLOG ( getName() + " FrameStack Error: " + MyString( getFrameStackSize() ) + " frames left" );
	}
	if (tmpFrame) delete tmpFrame;
}
//------------------------------------------------------------------------------------------------------------
void ProcessorNode::prepareFrameContainer( size_t num ) {
	size_t c = abs( (int)num - (int)frameContainer.size() );
	bool add = num > frameContainer.size();
	while ( c-- > 0 ) {
		if (add) frameContainer.push_back ( Frame::Ptr( new Frame() ) );
		else frameContainer.pop_back();
	}
}
//------------------------------------------------------------------------------------------------------------
void ProcessorNode::processNode( Processor::Int sampleFrames ) {
	processFrame ( mixInputsToFrame(sampleFrames), sampleFrames );
}
//============================================================================================================
// class ProcessAdapterNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ProcessAdapterNode::ProcessAdapterNode(ProcessAdapter * processAdapter) {
	adapter = processAdapter;
}
//------------------------------------------------------------------------------------------------------------
ProcessAdapterNode::~ProcessAdapterNode() {}
//============================================================================================================
// class NOPNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
NOPNode::NOPNode(const string &name) : ProcessorNode ( name ){}
//------------------------------------------------------------------------------------------------------------
void NOPNode::processFrame ( Frame *fr, Processor::Int sampleFrames ){
	pushAndCopy ( fr, sampleFrames );
}
//============================================================================================================
// class EndNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void EndNode::processNode( Processor::Int sampleFrames ) {
	Parents::iterator it = parents.begin();
	ProcessorNode *n;
	for ( ; it!=parents.end(); ++it ) {
		n = *it;
		if ( !n->isActive() ) continue;
		stream.addFrame ( n->popFrame(), sampleFrames,  getNodeDelay() - n->getNodeDelay() );
	}
}
//============================================================================================================
// class ProcessAdapter::InputNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ProcessAdapter::InputNode::InputNode ( const string &name, ProcessAdapter *parent ) :
	NOPNode (name), 
	parent(parent)
{
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
ProcessAdapter::ProcessAdapter( IHostInfo * hostInfo, size_t numInputNodes , size_t numOutputNodes ) : 
	hostInfo(hostInfo), 
	inputNodes( InputNodes (numInputNodes) ), 
	outputNodes( OutputNodes(numOutputNodes) )
{
	aNode = ProcessAdapterNode::create ( this ); // zuerst !!
	for ( int i=0; i<numInputNodes; ++i ) {
		InputNode::Ptr iNode = InputNode::create( "ProcessAdapter InputNode(" + MyString(i+1) + ")", this );
		inputNodes[i] = iNode;
	}

	for ( int i=0; i<numOutputNodes; ++i ) {
		OutputNode::Ptr oNode = OutputNode::create( "ProcessAdapter OutputNode(" + MyString(i+1) + ")", this );
		outputNodes[i] = oNode;
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
ProcessAdapter::~ProcessAdapter() {
}
//------------------------------------------------------------------------------------------------------------
size_t ProcessAdapter::getNumActiveOutputNodes() const {
	return aNode->getNumActiveChildren();
}
} // namespace processing