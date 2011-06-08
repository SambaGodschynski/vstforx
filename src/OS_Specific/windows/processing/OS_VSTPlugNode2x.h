#ifndef WIN_VST2XPLUGNODE_H
#define WIN_VST2XPLUGNODE_H



#include <string>
#include "audioEffectX.h"
#include "com/one4All.h"
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
	typedef HMODULE Module;
	//--------------------------------------------------------------------------------------------------------
	typedef std::pair< AudioMasterCallback, AudioEffectX* > HostCallBackOnInit; 
private:
	//--------------------------------------------------------------------------------------------------------
	static com::Mutex onInitLock;
	//--------------------------------------------------------------------------------------------------------
	// DLL Handler
	Module module;
	//--------------------------------------------------------------------------------------------------------
	string moduleLocation;
protected:
	//--------------------------------------------------------------------------------------------------------
	static HostCallBackOnInit callBkOnInit;
	//--------------------------------------------------------------------------------------------------------
	// Zeiger auf AEffect Klasse
	AEffect *aEff;
	//--------------------------------------------------------------------------------------------------------
	// blockiert mit mutex bis fertig geladen da statische variable callBkOnInit benutzt wird
	bool loadModule ( const HostCallBackOnInit &callBkOnInit );
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
	const Module & getModule() const { return module; }
	//--------------------------------------------------------------------------------------------------------
	OS_VSTPlugNode2x ( const string &moduleLocation ) : 
	  moduleLocation(moduleLocation), aEff(NULL), module(NULL){}
	//--------------------------------------------------------------------------------------------------------
	AEffect * getAEffect (){ return aEff; }

}; // class VSTPlugNode
} // namespace processing

#endif


