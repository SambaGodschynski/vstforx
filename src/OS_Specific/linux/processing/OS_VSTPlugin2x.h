/*
 * =============================================================================
 * OS_VSTPlugin2x.h
 *      Author: Johannes Unger
 * =============================================================================
 */

#ifndef LINUX_VST2XPLUGNODE_H
#define LINUX_VST2XPLUGNODE_H

#include <string>
#include "audioeffectx.h"
#include "com/one4All.h"

using namespace std;


namespace processing {
//==============================================================================
// Klasse: OS_VSTPlugNode2x.
// Laed plugin handler.
//==============================================================================
class OS_VSTPlugNode2x {
	friend class boost::serialization::access;
public:
private:
	//--------------------------------------------------------------------------
	std::string moduleLocation;
protected:
	//--------------------------------------------------------------------------
	AEffect * aEff;
public:
	//--------------------------------------------------------------------------
	OS_VSTPlugNode2x ( const string &moduleLocation="" ) {}
	//--------------------------------------------------------------------------
	AEffect * getAEffect (){ return aEff; }
	//--------------------------------------------------------------------------
	typedef std::pair< audioMasterCallback, AudioEffectX* > HostCallBackOnInit; 
	//--------------------------------------------------------------------------
	static HostCallBackOnInit callBkOnInit;
	//--------------------------------------------------------------------------
	bool loadModule ( const HostCallBackOnInit & ) { return false; }
	//--------------------------------------------------------------------------
	bool unloadModule () { return false; }
	//--------------------------------------------------------------------------
	void setModuleLocation ( const string &_moduleLocation ) { }
	//--------------------------------------------------------------------------
	const string & getModuleLocation () const { return moduleLocation; }
	//--------------------------------------------------------------------------
	int shellPlugId;
	//--------------------------------------------------------------------------
	static int shellPlugIdOnInit; // setted during loadModule for hostcallack on init

}; // class VSTPlugin
} // namespace processing

#endif


