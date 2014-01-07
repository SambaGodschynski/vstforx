/*
 * ============================================================================
 * VSTPlugin2x.h
 *      Author: Johannes Unger
 * ============================================================================
 */
#ifndef _VST2XPLUGNODE_H
#define _VST2XPLUGNODE_H

#include "com/one4All.h"
#include "boost/unordered_map.hpp"
#include "OS_Specific/os_processing.h"
#include "NullAEffect.h"
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <sambag/dsp/VstMidiEventAdapter.hpp>
#include "VstShellPlugin.hpp"
#include <sambag/com/ArithmeticWrapper.hpp>
#include "PluginImpl.hpp"

namespace processing{
using namespace com;
using namespace processing;
using namespace parameter;
//=============================================================================
/**
 * @class: VSTPluginImpl.
 * Represaentriert ein VST-Plugin.
 */
class VSTPluginImpl: 
	public OS_VSTPlugNode2x, // Plattformspezifische impl.
	public frx::processing::APluginImpl,
	public Serializable
{
//=============================================================================
friend class boost::serialization::access;

BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<VSTPluginImpl> Ptr;
	//-------------------------------------------------------------------------
	/**
	 * @param fileName
	 * @return Liefert Pluginname aus Speicherort. Zb.:
	 * C:/VSTPlugin.dll => VSTPlugin
	 */
	static MyString extractNameFromFilename ( const string &fileName );
private:
    //-------------------------------------------------------------------------
    mutable sambag::com::ArithmeticWrapper<size_t> _processDelay;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<sambag::dsp::VstMidiEventAdapter> VstMidiEventAdapterPtr;
	VstMidiEventAdapterPtr tmpMidiData;
	//-------------------------------------------------------------------------
	/**
	 * plugin calls ioChanged.
	 */
	void onIOChanged();
	//-------------------------------------------------------------------------
	/**
	 * Plugin-Editor Parameter. Handler
	 * @param aEff
	 * @param index
	 * @param value
	 */
	void onEditorParameterChanged ( int index, float value ); 
    //-------------------------------------------------------------------------
	/**
	 * VST-Plugin Editorfenster-Resize Callbackmethode
	 * @param effect
	 * @param w
	 * @param h
	 */
	void onPlugRequestWindowResize (size_t w, size_t h);
	//-------------------------------------------------------------------------
	typedef vector<string> ProgramNames;
	//-------------------------------------------------------------------------
	ProgramNames programNames;
	//-------------------------------------------------------------------------
	/**
	 * initalisiert Program-Namen (aka.Presets)
	 */
	void initProgramNames();
	//-------------------------------------------------------------------------
	/**
	 * Blockiert Deserialisierung gegen nebenlaufige
	 * Parameteraenderungen, verursacht durch Host.
	 */
	Mutex mutex;
	//-------------------------------------------------------------------------
	/**
	 * Blockiert ioChanged gegen process
	 */
	bool ioChangedLock;
	//-------------------------------------------------------------------------
	static VSTPluginImpl * getVSTPlugImpl ( AEffect *aEff ); // ermittelt ueber Aeffect=>vstplugnode map
	//-------------------------------------------------------------------------
	/**
	 * initalisiert Plugin-Parameter
	 */
	void initParameter();
	//-------------------------------------------------------------------------
	typedef boost::unordered_map < AEffect*, VSTPluginImpl* > RelatedPlugNode;
	//-------------------------------------------------------------------------
	/**
	 * Abbildung AEffect-Objektzeiger auf zugehoeriges VSTPlugin-Objekt.
	 * Wird fuer Host-Callbacks benoetigt.
	 */
	static RelatedPlugNode relatedPlugNode;
	//-------------------------------------------------------------------------
	bool canReceiveVstEvents;
	//-------------------------------------------------------------------------
	static void initPlug ( VSTPluginImpl &pln ); 
	//-------------------------------------------------------------------------
	/**
	 * boolsches Sperren von Parameteraenderungen.
	 * plug => parameter[index] => plug
	 */
	int onPlugChangeParameterIndex;
	//-------------------------------------------------------------------------
	void getShellPluginInfos(ShellPluginInfos &out);
protected:
	//-------------------------------------------------------------------------
	VSTPluginImpl( frx::processing::IHostInfo::Ptr hostInfo,
        Parameters *parameters,
        const string &filename
    );
public:
    //-------------------------------------------------------------------------
    virtual void openPlugin(frx::processing::IHostInfo::Ptr hi,
        const std::string &filename);
	//-------------------------------------------------------------------------
	/**
	 * @return Anzahl aller Plugin-Programme (aka. Presets)
	 */
	virtual size_t getNumPrograms();
	//-------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Program-Name zu index.
	 */
	virtual string getProgramName( size_t index );
	//-------------------------------------------------------------------------
	/**
	 * Aktiviert Program zu index.
	 * @param index
	 */
	virtual void setProgram( size_t index );
	//-------------------------------------------------------------------------
	/**
	 * @return index des akuell gesetzten Program, falls vorhanden. Andernfalls -1.
	 */
	virtual int getProgram();
	//-------------------------------------------------------------------------
	/**
	 * @return true, wenn von Client ausfuehrbar.
	 */
	virtual bool isAccessable() { return aEff != &nullAEff; }
	//-------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin Midi-Event verarbeiten kann.
	 */
	virtual bool canHandleMidiEvent() const { 
		return can(effFlagsIsSynth) || canReceiveVstEvents; 
	}
	//-------------------------------------------------------------------------
	/**
	 * Verarbeitet Midi-Events (@see VST-SDK VstEvents)
	 * @param events
	 */
	virtual void processMidiEvents( sambag::dsp::IMidiEvents * events );
	//-------------------------------------------------------------------------
	/**
	 * @return Signal-Verabeitungs-Verzoegerung des uebergeordneten ProcessAdapter
	 */
	virtual size_t getInitialDelay() const;
	//-------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge des Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int sampleFrames ) {}
	//-------------------------------------------------------------------------
	virtual ~VSTPluginImpl();
	//-------------------------------------------------------------------------
	/**
	 * Plugin-Parameter Handler
	 * @param src
	 * @param value
	 */
	virtual void valueChanged ( void *src, const float &value );
	//-------------------------------------------------------------------------
	/**
	 * aka. VST-Plugin: standby
	 */
	void turnOff() {
		aEff->dispatcher ( aEff, effMainsChanged, 0, 0, 0, 0 ); // turn off
	}
	//-------------------------------------------------------------------------
	/**
	 * aka. VST-Plugin: on
	 */
	void turnOn() {
		aEff->dispatcher ( aEff, effMainsChanged, 0, 1, 0, 0 ); // turn on
	}
	//-------------------------------------------------------------------------
	/**
	 * VST-Spezifische Reset-Sequenz
	 */
	inline void resetPlugin(){
		turnOff();
		turnOn();
	}
	//-------------------------------------------------------------------------
	virtual void openEditor(void *window);
	//-------------------------------------------------------------------------
	virtual void closeEditor(void *window);
	//-------------------------------------------------------------------------
	virtual void onEditorIdle();
	//-------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin ueber Editor verfuegt.
	 */
	virtual bool hasEditor() const {
		return can(effFlagsHasEditor);
	}
	//-------------------------------------------------------------------------
	/**
	 * Host-Info changed Handler
	 */
	virtual void baseConfigChanged(frx::processing::IHostInfo::Ptr hi);
	//-------------------------------------------------------------------------
	/**
	 * @param flag
	 * @return liefert Wahrheitswert zu VST-Plugin spezifiscer can-flags
	 */
	bool can ( VstInt32 flag ) const {
		return isFlag(flag, aEff->flags); 
	}
	//-------------------------------------------------------------------------
	/**
	 * @param flag
	 * @param aEff AEffect-Objektzeiger
	 * @return liefert Wahrheitswert zu VST-Plugin spezifiscer can-flags
	 */
	static bool can ( VstInt32 flag, AEffect *aEff ) {
		return isFlag(flag, aEff->flags); 
	}
	//-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    virtual void processPlugin(Frames::T **_in,
        Frames::T **_out, size_t numSamples);
    //-------------------------------------------------------------------------
    size_t getNumInputChannels() const {
        SAMBAG_ASSERT(aEff);
        return aEff->numInputs;
    }
    //-------------------------------------------------------------------------
    size_t getNumOutputChannels() const {
        SAMBAG_ASSERT(aEff);
        return aEff->numOutputs;
    }
}; // class VSTPluginImpl
} // namespace processing

#endif


