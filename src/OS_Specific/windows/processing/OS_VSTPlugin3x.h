/*
 * ===========================================================================================================
 * OS_VSTPlugin3x.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef WIN_VST3XPLUGNODE_H
#define WIN_VST3XPLUGNODE_H

#include <string>
#include "com/one4All.h"
#include "processing/processing.h"
#include <windows.h>
#include "pluginterfaces/base/ipluginbase.h"

using namespace std;

namespace processing {
//============================================================================================================
// Klasse: OS_VSTPlugNode3x.
// Laed plugin handler.
//============================================================================================================
class OS_VSTPlugNode3x {
	friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef HMODULE Module;
private:
	//--------------------------------------------------------------------------------------------------------
	// DLL Handler
	Module module;
	//--------------------------------------------------------------------------------------------------------
	string moduleLocation;
protected:
    Steinberg::IPluginFactory* factory;
	//--------------------------------------------------------------------------------------------------------
	bool loadModule ();
	//--------------------------------------------------------------------------------------------------------
	bool unloadModule ();
	//--------------------------------------------------------------------------------------------------------
	void setModuleLocation ( const string &_moduleLocation ) { moduleLocation = _moduleLocation; }
	//--------------------------------------------------------------------------------------------------------
	const string & getModuleLocation () const { return moduleLocation; }
	//--------------------------------------------------------------------------------------------------------
	OS_VSTPlugNode3x() : module(NULL), factory(NULL) {}
public:
	//--------------------------------------------------------------------------------------------------------
	OS_VSTPlugNode3x ( const string &moduleLocation ) :
		moduleLocation(moduleLocation), module(NULL), factory(NULL) {}

}; // class VSTPlugin
} // namespace processing

#endif

