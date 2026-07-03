/*
 * ===========================================================================================================
 * Plugin.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include <boost/bind.hpp>
#include "Plugin.h"
#include "OS_Specific/OS_processing.h"
#include "com/one4All.h"
#include <boost/filesystem.hpp>
#include <sambag/disco/Geometry.hpp>
#include <processing/Plugin.h> // new plugin

namespace processing {

enum { ALL_CHANNEL = 16 };

//============================================================================================================
// Plugin
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void Plugin::paramEditorPosXChanged ( void *src, const float &val ) {
    using namespace sambag::disco;
    typedef frx::processing::Plugin PluginMII;
    Point2D p((Coordinate)*editorPosX, (Coordinate)*editorPosY);
    sce::EventSender<sce::PropertyChanged>::notifyListeners (this,
			sce::PropertyChanged (PluginMII::PROPERTY_PARAMETER_EDITOR_POSITION, p, p)
		);
	}
//------------------------------------------------------------------------------------------------------------
void Plugin::paramEditorPosYChanged ( void *src, const float &val ) {
    using namespace sambag::disco;
    typedef frx::processing::Plugin PluginMII;
    Point2D p((Coordinate)*editorPosX, (Coordinate)*editorPosY);
    sce::EventSender<sce::PropertyChanged>::notifyListeners (this,
			sce::PropertyChanged (PluginMII::PROPERTY_PARAMETER_EDITOR_POSITION, p, p)
		);
	}
//------------------------------------------------------------------------------------------------------------
void Plugin::paramEditorOpenChanged ( void *src, const float &val ) {
    typedef frx::processing::Plugin PluginMII;
    sce::EventSender<sce::PropertyChanged>::notifyListeners (this,
			sce::PropertyChanged(PluginMII::PROPERTY_PARAMETER_EDITOR_OPENSTATE, val>0.5, val>0.5 )
		);
	}
//------------------------------------------------------------------------------------------------------------
void Plugin::setStatusMsg( const std::string &msg ) {
	statusMsg = msg;
}
//------------------------------------------------------------------------------------------------------------
void Plugin::initListener() {
    using ::processing::parameter::Parameter;
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
editorPosX ( ::processing::parameter::Parameter::create() ),
editorPosY ( ::processing::parameter::Parameter::create() ),
editorOpen ( ::processing::parameter::Parameter::create() )
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
}//namespace processing
