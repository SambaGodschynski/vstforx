/*
 * ===========================================================================================================
 * VSTPlugin2x.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef _VST2XPLUGNODE_H
#define _VST2XPLUGNODE_H

#include "com/one4All.h"
#include "processing/processing.h"
#include "processing/Plugin.h"
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
/**
 * @class: VSTPlugin.
 * Represaentriert ein VST-Plugin.
 */
class VSTPlugin: 
	public OS_VSTPlugNode2x, // Plattformspezifische impl.
	public Plugin, 
	public Serializable
{
//============================================================================================================
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
	/**
	 * initalisiert Program-Namen (aka.Presets)
	 */
	void initProgramNames();
	//--------------------------------------------------------------------------------------------------------
	Frames nullFrame; // fuer nicht genutzte eingaenge ( beim frame=>float[] )
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Deserialisert VSTPlugin.
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void load ( iArchive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisert VSTPlugin.
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void save ( oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
	VSTPlugin() : onPlugChangeParameterIndex(-1) {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Blockiert Deserialisierung gegen nebenlaufige
	 * Parameteraenderungen, verursacht durch Host.
	 */
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
	/**
	 * initalisiert Plugin-Parameter
	 */
	void initParameter();
	//--------------------------------------------------------------------------------------------------------
	typedef boost::unordered_map < AEffect*, VSTPlugin* > RelatedPlugNode;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Abbildung AEffect-Objektzeiger auf zugehoeriges VSTPlugin-Objekt.
	 * Wird fuer Host-Callbacks benoetigt.
	 */
	static RelatedPlugNode relatedPlugNode;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param fileName
	 * @return Liefert Pluginname aus Speicherort. Zb.:
	 * C:/VSTPlugin.dll => VSTPlugin
	 */
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
	/**
	 * INITALISIERT Framesbuffer
	 */
	void setupFramesbuffer();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * boolsches Sperren von Parameteraenderungen.
	 * plug => parameter[index] => plug
	 */
	int onPlugChangeParameterIndex;
protected:
	//--------------------------------------------------------------------------------------------------------
	VSTPlugin( IHostInfo *hostInfo, const string &filename );
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @param filename
	 * @return neues VSTPlugin-Objekt
	 */
	static Ptr create( IHostInfo *hostInfo, const string &filename ) {
		Ptr neu( new VSTPlugin(hostInfo, filename) );
		if ( !neu ) 
			return Ptr();
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller Plugin-Programme (aka. Presets)
	 */
	virtual size_t getNumPrograms();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Program-Name zu index.
	 */
	virtual string getProgramName( size_t index );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Aktiviert Program zu index.
	 * @param index
	 */
	virtual void setProgram( size_t index );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return index des akuell gesetzten Program, falls vorhanden. Andernfalls -1.
	 */
	virtual int getProgram();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn von Client ausfuehrbar.
	 */
	virtual bool isAccessable() { return aEff != &nullAEff; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin Midi-Event verarbeiten kann.
	 */
	virtual bool canHandleMidiEvent() const { 
		return can(effFlagsIsSynth) || canReceiveVstEvents; 
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Midi-Events (@see VST-SDK VstEvents)
	 * @param events
	 */
	virtual void processMidiEvents( VstEvents * events );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Signal-Verabeitungs-Verzoegerung des uebergeordneten ProcessAdapter
	 */
	virtual size_t getProcessDelay() const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge des Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int sampleFrames );
	//--------------------------------------------------------------------------------------------------------
	virtual ~VSTPlugin();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Plugin-Parameter Handler
	 * @param src
	 * @param value
	 */
	virtual void valueChanged ( void *src, const float &value );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Plugin-Editor Parameter. Handler
	 * @param aEff
	 * @param index
	 * @param value
	 */
	static void editorParameterChanged ( AEffect *aEff, int index, float value ); 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return liefert Parameter zu index. Wirft: std::out_of_range
	 */
	virtual Parameter::Ptr getParameter ( size_t index ) const {
		return param.at( index);
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * aka. VST-Plugin: standby
	 */
	void turnOff() {
		aEff->dispatcher ( aEff, effMainsChanged, 0, 0, 0, 0 ); // turn off
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * aka. VST-Plugin: on
	 */
	void turnOn() {
		aEff->dispatcher ( aEff, effMainsChanged, 0, 1, 0, 0 ); // turn on
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * VST-Spezifische Reset-Sequenz
	 */
	inline void resetPlugin(){
		turnOff();
		turnOn();
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller MidiProcessor-Parameter
	 */
	virtual size_t getNumParameter() const {
		return param.size();	
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin ueber Editor verfuegt.
	 */
	virtual bool hasEditor() const {
		return can(effFlagsHasEditor);
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Host-Info changed Handler
	 */
	virtual void hostInfoChanged();
	///-------------------------------------------------------------------------------------------------------
	/**
	 * @param flag
	 * @return liefert Wahrheitswert zu VST-Plugin spezifiscer can-flags
	 */
	bool can ( VstInt32 flag ) const {
		return isFlag(flag, aEff->flags); 
	}
	///-------------------------------------------------------------------------------------------------------
	/**
	 * @param flag
	 * @param aEff AEffect-Objektzeiger
	 * @return liefert Wahrheitswert zu VST-Plugin spezifiscer can-flags
	 */
	static bool can ( VstInt32 flag, AEffect *aEff ) {
		return isFlag(flag, aEff->flags); 
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * VST-Plugin Editorfenster-Resize Callbackmethode
	 * @param effect
	 * @param w
	 * @param h
	 */
	static void plugRequestWindowResize (  AEffect* effect, size_t w, size_t h );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Host->Plugin Callbackmethode. (@see VST-SDK)
	 * @param effect AEffect-Objektzeiger
	 * @param opcode Anfragetyp
	 * @param index  Anfragetyp-Spezifischer Index
	 * @param value Anfragetyp-Spezifischer Wert
	 * @param ptr Anfragetyp-Spezifischer Zeiger
	 * @param opt Anfragetyp-Spezifischer Fliesskommawert
	 * @return Anfragetyp-Spezifisch
	 */
	static VstIntPtr _hostCallback ( AEffect* effect, 
									VstInt32 opcode, 
									VstInt32 index, 
									VstIntPtr value, 
									void* ptr, 
									float opt ); 
}; // class VSTPlugin
} // namespace processing

#endif


