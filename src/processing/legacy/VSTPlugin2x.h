/*
 * ===========================================================================================================
 * VSTPlugin2x.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FRX_LEGACY_VST2XPLUGNODE_H
#define FRX_LEGACY_VST2XPLUGNODE_H

#include "com/one4All.h"
#include "processing/processing.h"
#include "Plugin.h"
#include "processing/parameter/parameter.h"
#include "boost/unordered_map.hpp"
#include "OS_Specific/OS_processing.h"
#include <processing/pluginTypes/NullAEffect.h>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <sambag/dsp/VstMidiEventAdapter.hpp>
#include <processing/pluginTypes/VstShellPlugin.hpp>
#include <sambag/com/ArithmeticWrapper.hpp>

/*namespace legacy {*/ namespace processing {
//============================================================================================================
/**
 * @class: VSTPlugin.
 * Represaentriert ein VST-Plugin.
 */
class VSTPlugin: 
	public ::processing::OS_VSTPlugNode2x, // Plattformspezifische impl.
	public Plugin,
	public ::com::Serializable
{
//============================================================================================================
friend class boost::serialization::access;

BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<VSTPlugin> Ptr;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param fileName
	 * @return Liefert Pluginname aus Speicherort. Zb.:
	 * C:/VSTPlugin.dll => VSTPlugin
	 */
	static ::com::MyString extractNameFromFilename ( const string &fileName );
private:
    //--------------------------------------------------------------------------------------------------------
    mutable sambag::com::ArithmeticWrapper<size_t> _processDelay;
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<sambag::dsp::VstMidiEventAdapter> VstMidiEventAdapterPtr;
	VstMidiEventAdapterPtr tmpMidiData;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * plugin calls ioChanged.
	 */
	void onIOChanged();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Plugin-Editor Parameter. Handler
	 * @param aEff
	 * @param index
	 * @param value
	 */
	void onEditorParameterChanged ( int index, float value ); 
		//--------------------------------------------------------------------------------------------------------
	/**
	 * VST-Plugin Editorfenster-Resize Callbackmethode
	 * @param effect
	 * @param w
	 * @param h
	 */
	void onPlugRequestWindowResize ( size_t w, size_t h);
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
	::processing::Frames nullFrame; // fuer nicht genutzte eingaenge ( beim frame=>float[] )
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Deserialisert VSTPlugin.
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void load ( ::com::iArchive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisert VSTPlugin.
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void save ( ::com::oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
	VSTPlugin() : onPlugChangeParameterIndex(-1), ioChangedLock(false) {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Blockiert Deserialisierung gegen nebenlaufige
	 * Parameteraenderungen, verursacht durch Host.
	 */
	com::Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Blockiert ioChanged gegen process
	 */
	bool ioChangedLock;
	//--------------------------------------------------------------------------------------------------------
	typedef vector< ::processing::parameter::Parameter::Ptr > ParameterContainer;
	//--------------------------------------------------------------------------------------------------------
	ParameterContainer param;
	//--------------------------------------------------------------------------------------------------------
	int blockSize;
	//--------------------------------------------------------------------------------------------------------
	typedef vector< ::processing::Frames > Framebuffer;
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
	//--------------------------------------------------------------------------------------------------------
	void getShellPluginInfos(::processing::ShellPluginInfos &out);
    //--------------------------------------------------------------------------------------------------------
    /**
     * recognized testplugin via filename: frx.vst2x.FrxTestplugin(numInputs, numOutputs)
     */
    void initAsTestPluginIfNecessary(std::string filename);
protected:
	//--------------------------------------------------------------------------------------------------------
	VSTPlugin( frx::processing::IHostInfo::Ptr hostInfo, const string &filename );
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void setPresetData(const std::string &data);
	//--------------------------------------------------------------------------------------------------------
	virtual std::string getPresetData();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @param filename
	 * @return neues VSTPlugin-Objekt
	 */
	static Ptr create( frx::processing::IHostInfo::Ptr hostInfo, const string &filename ) {
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
	virtual bool isAccessable() { return aEff != &::processing::nullAEff; }
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
	virtual void processMidiEvents( sambag::dsp::IMidiEvents::Ptr events );
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
	virtual void processAdapter( ::processing::Processor::Int sampleFrames );
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
	 * @param index
	 * @return liefert Parameter zu index. Wirft: std::out_of_range
	 */
	virtual ::processing::parameter::Parameter::Ptr getParameter ( size_t index ) const {
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
    virtual void beforeOpenEditor(sambag::disco::components::WindowPtr win);
	//--------------------------------------------------------------------------------------------------------
	virtual void openEditor(sambag::disco::components::WindowPtr win);
	//--------------------------------------------------------------------------------------------------------
	virtual void closeEditor(sambag::disco::components::WindowPtr win);
	//--------------------------------------------------------------------------------------------------------
	virtual void onEditorIdle();
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
	virtual void hostBaseConfigChanged();
	///-------------------------------------------------------------------------------------------------------
	/**
	 * @param flag
	 * @return liefert Wahrheitswert zu VST-Plugin spezifiscer can-flags
	 */
	bool can ( VstInt32 flag ) const {
		return ::com::isFlag(flag, aEff->flags);
	}
	///-------------------------------------------------------------------------------------------------------
	/**
	 * @param flag
	 * @param aEff AEffect-Objektzeiger
	 * @return liefert Wahrheitswert zu VST-Plugin spezifiscer can-flags
	 */
	static bool can ( VstInt32 flag, AEffect *aEff ) {
		return ::com::isFlag(flag, aEff->flags);
	}
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


