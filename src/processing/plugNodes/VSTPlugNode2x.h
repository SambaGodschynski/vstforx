#ifndef _VST2XPLUGNODE_H
#define _VST2XPLUGNODE_H

#include "com/one4All.h"
#include "processing/processing.h"
#include "processing/PlugNode.h"
#include "processing/parameter/parameter.h"
#include "boost/unordered_map.hpp"
#include "OS_Specific/os_processing.h"
#include "NullAEffect.h"
#include <string>
#include <vector>
#include <list>
#include <stack>


namespace ppiGui {
	class GPluginController;
}

namespace processing{
using namespace com;
using namespace processing;
using namespace parameter;
//============================================================================================================
// Klasse: VSTPlugin.
// Represaentriert ein VST-Plugin.
//============================================================================================================
class VSTPlugin: 
	public OS_VSTPlugNode2x,
	public Plugin, 
	public ValueChangedListener<float>,
	public Serializable
{
friend class boost::serialization::access;
friend class ppiGui::GPluginController;

BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<VSTPlugin> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	typedef vector<string> ProgramNames;
	//--------------------------------------------------------------------------------------------------------
	ProgramNames programNames;
	//--------------------------------------------------------------------------------------------------------
	void initProgramNames();
	//--------------------------------------------------------------------------------------------------------
	Frames nullFrame; // fuer nicht genutzte eingaenge ( beim frame=>float[] )
	//--------------------------------------------------------------------------------------------------------
	void load ( iArchive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	void save ( oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
	VSTPlugin() : onPlugChangeParameterIndex(-1) {}
	//--------------------------------------------------------------------------------------------------------
	Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<Parameter::Ptr> ParameterContainer;
	//--------------------------------------------------------------------------------------------------------
	ParameterContainer param;
	//--------------------------------------------------------------------------------------------------------
	int blockSize;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<Frames> Framebuffer;
	//--------------------------------------------------------------------------------------------------------
	Framebuffer framebuffer;
	//--------------------------------------------------------------------------------------------------------
	static VSTPlugin * getVSTPlugNode ( AEffect *aEff ); // ermittelt ueber Aeffect=>vstplugnode map
	//--------------------------------------------------------------------------------------------------------
	void initParameter();
	//--------------------------------------------------------------------------------------------------------
	typedef boost::unordered_map < AEffect*, VSTPlugin* > RelatedPlugNode;
	//--------------------------------------------------------------------------------------------------------
	static RelatedPlugNode relatedPlugNode;
	//--------------------------------------------------------------------------------------------------------
	static MyString extractNameFromFilename ( const string &fileName );
	//--------------------------------------------------------------------------------------------------------
	bool canReceiveVstEvents;
	//--------------------------------------------------------------------------------------------------------
	float ** inMatrix;
	//--------------------------------------------------------------------------------------------------------
	float ** outMatrix;
	//--------------------------------------------------------------------------------------------------------
	void initInputs();
	//--------------------------------------------------------------------------------------------------------
	void initOutputs();
	//--------------------------------------------------------------------------------------------------------
	static void initPlug ( VSTPlugin &pln ); 
	//--------------------------------------------------------------------------------------------------------
	void setupFramebuffer();
	//--------------------------------------------------------------------------------------------------------
	int onPlugChangeParameterIndex; // sperre plug => parameter => plug
protected:
	//--------------------------------------------------------------------------------------------------------
	VSTPlugin( IHostInfo *hostInfo, const string &filename );
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( IHostInfo *hostInfo, const string &filename ) {
		Ptr neu( new VSTPlugin(hostInfo, filename) );
		if ( !neu ) return Ptr();
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumPrograms();
	//--------------------------------------------------------------------------------------------------------
	virtual string getProgramName( size_t index );
	//--------------------------------------------------------------------------------------------------------
	virtual void setProgram( size_t index );
	//--------------------------------------------------------------------------------------------------------
	virtual int getProgram();
	//--------------------------------------------------------------------------------------------------------
	virtual bool isAccessable() { return aEff != &nullAEff; }
	//--------------------------------------------------------------------------------------------------------
	virtual bool canHandleMidiEvent() const { 
		return can(effFlagsIsSynth) || canReceiveVstEvents; 
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void processMidiEvents( VstEvents * events );
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getProcessDelay() const;
	//--------------------------------------------------------------------------------------------------------
	virtual void _processAdapter( Processor::Int sampleFrames );
	//--------------------------------------------------------------------------------------------------------
	virtual ~VSTPlugin();
	//--------------------------------------------------------------------------------------------------------
	virtual void valueChanged ( void *src, const float &value );
	//--------------------------------------------------------------------------------------------------------
	static void editorParameterChanged ( AEffect *aEff, int index, float value ); 
	//--------------------------------------------------------------------------------------------------------
	virtual Parameter::Ptr getParameter ( size_t index ) const {
		return param[index];
	}
	//--------------------------------------------------------------------------------------------------------
	void turnOff() {
		aEff->dispatcher ( aEff, effMainsChanged, 0, 0, 0, 0 ); // turn off
	}
	//--------------------------------------------------------------------------------------------------------
	void turnOn() {
		aEff->dispatcher ( aEff, effMainsChanged, 0, 1, 0, 0 ); // turn on
	}
	//--------------------------------------------------------------------------------------------------------
	inline void resetPlugin(){
		turnOff();
		turnOn();
	}
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter() const {
		return param.size();	
	}
	//--------------------------------------------------------------------------------------------------------
	virtual bool hasEditor() const {
		return can(effFlagsHasEditor);
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void hostInfoChanged();
	///-------------------------------------------------------------------------------------------------------
	// Liefert VSTPlugIn wahrheitswert von flag.
	bool can ( VstInt32 flag ) const {
		return isFlag(flag, aEff->flags); 
	}
	///-------------------------------------------------------------------------------------------------------
	// Liefert VSTPlugIn wahrheitswert von flag.
	static bool can ( VstInt32 flag, AEffect *aEff ) {
		return isFlag(flag, aEff->flags); 
	}
	//--------------------------------------------------------------------------------------------------------
	static void plugRequestWindowResize (  AEffect* effect, size_t w, size_t h );
	//--------------------------------------------------------------------------------------------------------
	static VstIntPtr _hostCallback ( AEffect* effect, 
									VstInt32 opcode, 
									VstInt32 index, 
									VstIntPtr value, 
									void* ptr, 
									float opt ); 
}; // class VSTPlugin
} // namespace processing

#endif


