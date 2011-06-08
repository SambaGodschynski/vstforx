#include "PlugNode.h"
#include "OS_Specific/os_processing.h"
#include "plugNodes/VSTPlugNode2x.h"
#include "com/One4All.h"
#include <boost/filesystem.hpp>

namespace processing{

enum { ALL_CHANNEL = 16 };
	
//============================================================================================================
// PlugNode
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
PlugNode::PlugNode ( IHostInfo *hostInfo, const string &location, size_t numInputs , size_t numOutputs ) :
ProcessAdapter ( hostInfo, numInputs, numOutputs ),
editorPosX ( processing::parameter::Parameter::create() ),
editorPosY ( processing::parameter::Parameter::create() ),
editorOpen ( processing::parameter::Parameter::create() )
{
	setLocation ( location );

	// init editorPos parameters
	editorPosX->setName("editor_X");
	editorPosY->setName("editor_Y");
	editorOpen->setName("editor open");
	*editorPosX = 0.72f; // 0.5 = 0 SCREEN_X
	*editorPosY = 0.72f; // 0.5 = 0 SCREEN_Y
	// register editor pos parameter in plugin
	parameter::Parameter::ParameterListenerFunction xC = boost::bind( 
		&PlugNode::paramEditorPosXChanged, this, _1, _2 
	);
	parameter::Parameter::ParameterListenerFunction yC = boost::bind( 
		&PlugNode::paramEditorPosYChanged, this, _1, _2 
	);
	Parameter::ParameterListenerFunction oC = boost::bind( 
		&PlugNode::paramEditorOpenChanged, this, _1, _2 
	);
	Parameter::ParameterListenerFunction dC = boost::bind( 
		&PlugNode::paramEditorOpenDisplayChanged, this, _1, _2 
	);
	editorPosX->addValueChangedListenerF( xC );
	editorPosY->addValueChangedListenerF( yC );
	editorOpen->addValueChangedListenerF( oC );
	editorOpen->addValueChangedListenerF( dC );
	*editorOpen = 0.0f;
}
//------------------------------------------------------------------------------------------------------------
PlugNode::~PlugNode() {
	// unregister editor pos parameter in plugin
	Parameter::ParameterListenerFunction xC = boost::bind( 
		&PlugNode::paramEditorPosXChanged, this, _1, _2 
	);
	Parameter::ParameterListenerFunction yC = boost::bind( 
		&PlugNode::paramEditorPosYChanged, this, _1, _2 
	);
	Parameter::ParameterListenerFunction oC = boost::bind( 
		&PlugNode::paramEditorOpenChanged, this, _1, _2 
	);
	editorPosX->removeValueChangedListenerF( xC );
	editorPosY->removeValueChangedListenerF( yC );
	editorOpen->removeValueChangedListenerF( oC );
}
//============================================================================================================
// Klasse: PluginFactory.
// erzeugt plugin.
//============================================================================================================
//--------------------------------------------------------------------------------------------------------
PlugNode::Ptr PluginFactory::createVST2xPlugNode ( IHostInfo *hostInfo, const string &filename ) {
	return VSTPlugNode::create( hostInfo, filename );
	
	// ... weitere Plugs TODO: VST3.x

}
//------------------------------------------------------------------------------------------------------------
PlugNode::Ptr PluginFactory::createPlugNode (  IHostInfo *hostInfo, const string &filename ) {
	return createVST2xPlugNode ( hostInfo, filename ); 
	/*
	switch ( pI.pluginType ) {
		case PluginInfo::UNKNOWN :
			throw com::ppiError::DllError ( "unkown plug.", __FILE__, __LINE__ ); break;
		case PluginInfo::VST2X :
		default:
			return NULL;
	}*/
}


}//namespace processing
