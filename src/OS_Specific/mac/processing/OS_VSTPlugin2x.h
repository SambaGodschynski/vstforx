/*
 * ===========================================================================================================
 * OS_VSTPlugin2x.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef MAC_VST2XPLUGNODE_H
#define MAC_VST2XPLUGNODE_H



#include <string>
#include "audioEffectX.h"
#include "com/one4All.h"
#include "CoreFoundation/CoreFoundation.h"
#include "processing/processing.h"

using namespace std;


namespace processing {
//============================================================================================================
// Klasse: OS_VSTPlugNode2x.
// Laed plugin handler.
//============================================================================================================
class OS_VSTPlugNode2x {
	friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef CFBundleRef Module;
	//--------------------------------------------------------------------------------------------------------
	typedef std::pair< AudioMasterCallback, AudioEffectX* > HostCallBackOnInit; 
private:
	//--------------------------------------------------------------------------------------------------------
	string moduleLocation;
	//--------------------------------------------------------------------------------------------------------
	static com::Mutex onInitLock;
protected:
	//--------------------------------------------------------------------------------------------------------
	static HostCallBackOnInit callBkOnInit;
	//--------------------------------------------------------------------------------------------------------
	// Zeiger auf AEffect Klasse
	AEffect *aEff;
	//--------------------------------------------------------------------------------------------------------
	Module module;
	//--------------------------------------------------------------------------------------------------------
	bool loadModule ( const HostCallBackOnInit & );
	//--------------------------------------------------------------------------------------------------------
	bool unloadModule ();
	//--------------------------------------------------------------------------------------------------------
	void setModuleLocation ( const string &_moduleLocation ) { moduleLocation = _moduleLocation; }
	//--------------------------------------------------------------------------------------------------------
	const string & getModuleLocation () const { return moduleLocation; }
	//--------------------------------------------------------------------------------------------------------
	OS_VSTPlugNode2x() {}
public:
	//--------------------------------------------------------------------------------------------------------
	OS_VSTPlugNode2x ( const string &moduleLocation ) : 
	moduleLocation(moduleLocation), aEff(NULL), module(NULL) {}
	//--------------------------------------------------------------------------------------------------------
	AEffect * getAEffect (){ return aEff; }

}; // class VSTPlugin
} // namespace processing

#endif


