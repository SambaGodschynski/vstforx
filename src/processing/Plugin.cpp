/*
 * ===========================================================================================================
 * Plugin.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "Plugin.h"
#include "OS_Specific/os_processing.h"
#include "pluginTypes/VSTPlugin2x.h"
#include "com/One4All.h"
#include <boost/filesystem.hpp>

namespace processing{

enum { ALL_CHANNEL = 16 };
	
//============================================================================================================
// Plugin
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void Plugin::setStatusMsg( const std::string &msg ) {
	statusMsg = msg;
}
//------------------------------------------------------------------------------------------------------------
void Plugin::initListener() {
	Parameter::ParameterListenerFunction xC = boost::bind( 
		&Plugin::paramEditorPosXChanged, this, _1, _2 
	);
	Parameter::ParameterListenerFunction yC = boost::bind( 
		&Plugin::paramEditorPosYChanged, this, _1, _2 
	);
	Parameter::ParameterListenerFunction oC = boost::bind( 
		&Plugin::paramEditorOpenChanged, this, _1, _2 
	);
	Parameter::ParameterListenerFunction dC = boost::bind( 
		&Plugin::paramEditorOpenDisplayChanged, this, _1, _2 
	);
	editorPosX->addValueChangedListener(xC);
	editorPosY->addValueChangedListener(yC);
	paramEditorOpenConnection = editorOpen->addValueChangedListener(oC);
	editorOpen->addValueChangedListener(dC);
}
//------------------------------------------------------------------------------------------------------------
Plugin::Plugin ( frx::processing::IHostInfo::Ptr hostInfo, const string &location, size_t numInputs , size_t numOutputs ) :
ProcessAdapter ( hostInfo, numInputs, numOutputs ),
editorPosX ( processing::parameter::Parameter::create() ),
editorPosY ( processing::parameter::Parameter::create() ),
editorOpen ( processing::parameter::Parameter::create() )
{
	setLocation ( location );

	// init editorPos parameters
	editorPosX->setName("editor_X");
	editorPosY->setName("editor_Y");
	editorOpen->setName("editor_visibility");
	*editorPosX = 0.72f; // 0.5 = 0 SCREEN_X
	*editorPosY = 0.72f; // 0.5 = 0 SCREEN_Y
	*editorOpen = 0.0f;
	initListener();
}
//------------------------------------------------------------------------------------------------------------
Plugin::~Plugin() {
}
//============================================================================================================
// Klasse: PluginFactory.
// erzeugt plugin.
//============================================================================================================
//--------------------------------------------------------------------------------------------------------
Plugin::Ptr PluginFactory::createVST2xPlugNode ( frx::processing::IHostInfo::Ptr hostInfo, const string &filename ) {
	return VSTPlugin::create( hostInfo, filename );
	
	// ... weitere Plugs TODO: VST3.x

}
//------------------------------------------------------------------------------------------------------------
Plugin::Ptr PluginFactory::createPlugNode (  frx::processing::IHostInfo::Ptr hostInfo, const string &filename ) {
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
