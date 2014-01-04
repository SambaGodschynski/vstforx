/*
 * ============================================================================
 * Plugin.h
 *      Author: Johannes Unger
 * ============================================================================
 */
#ifndef PLUGNODE_H
#define PLUGNODE_H


#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/one4All.h"
#include "com/Serialization.h"
#include "com/Events.h"
#include "PlugInfo.h"
#include "processing/MidiEventProcessor.h"


namespace processing {
//=============================================================================
/**
 * @class: ResizeEditor.
 * Event: wird aufgerufen wenn Plugin, Editor-Resize, anfordert.
 */
struct ResizeEditorEvent : public com::events::Event {
//=============================================================================
	size_t w, h;
	ResizeEditorEvent ( size_t width, size_t height ) : w(width), h(height) {}
};
//-----------------------------------------------------------------------------
typedef std::pair< float, float > EditorPosition; 
//-----------------------------------------------------------------------------
typedef com::events::ValueChangedEvent<EditorPosition> EditorPositionEvent;
//=============================================================================
/**
 * @class EditorOpenParameterChanged.
 * Event: wird aufgerufen wenn Plugin-Editor, Open/Close-Parameter geandert
 */
struct EditorOpenParameterChanged : public com::events::Event {
//=============================================================================
	bool open;
	EditorOpenParameterChanged( bool open ) : open(open) {}
};
//=============================================================================
/**
 * Klasse: Plugin.
 * Oberklasse fuer Plugin.
 */
class Plugin: 
	public ProcessAdapter,
	public parameter::HasParameter,
	public MidiEventProcessor,
	public com::events::EventSender<EditorPositionEvent>,
	public com::events::EventSender<EditorOpenParameterChanged>,
	public com::events::EventSender<ResizeEditorEvent>
{
//=============================================================================
friend class boost::serialization::access;
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<Plugin> Ptr;
private:
	//-------------------------------------------------------------------------
	std::string statusMsg;
	//-------------------------------------------------------------------------
	parameter::Parameter::Connection paramEditorOpenConnection;
	//-------------------------------------------------------------------------
	PluginInfo pluginInfo;
	//-------------------------------------------------------------------------
	std::string plugVendor;
	//-------------------------------------------------------------------------
	// editor parameter:
	// They will be processed by GObjectController. To save their states independendly from view,
	// they are stored here and not in VSTPlugView.
	processing::parameter::Parameter::Ptr editorPosX, editorPosY, editorOpen;
	//-------------------------------------------------------------------------
	/**
	 * (De)Serialisiert Plugin-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template <typename Archive>
	void serialize ( Archive &ar, const unsigned int version ) {
		using namespace processing::parameter;
		ar & boost::serialization::base_object<ProcessAdapter> ( *this );
		ar & boost::serialization::base_object<MidiEventProcessor> ( *this );
		ar & pluginInfo;
		ar & plugVendor;
		ar & editorPosX;
		ar & editorPosY;
		ar & editorOpen;
		if ( Archive::is_loading::value ) {
			initListener();
		}
	}
protected:
	//-------------------------------------------------------------------------
	/**
	 * Initalisiert Listener.
	 */
	void initListener();
	//-------------------------------------------------------------------------
	Plugin() {}
	//-------------------------------------------------------------------------
	Plugin( frx::processing::IHostInfo::Ptr hostInfo, const std::string &location,
        size_t numInputs = 1, size_t numOutputs = 1 );
public:
    //-------------------------------------------------------------------------
    /**
     * @brief alternative process call, used by bridged plugins.
     */
    virtual void process(const Frames &_in, Frames &_out, size_t numSamples) = 0;
    //-------------------------------------------------------------------------
    /**
     * @override
     */
    virtual std::string getStatusMessage() const { return statusMsg; }
	//-------------------------------------------------------------------------
	void setStatusMsg( const std::string &msg );
	//-------------------------------------------------------------------------
	/**
	 * Wird von GPluginController benoetigt um bei bedarf verbindung zu blockieren.
	 * TODO: schlechte Loesung!
	 * @return Parameter-EditorOpenChanged-Connection
	 */
	const parameter::Parameter::Connection & getParamEditorOpenConnection() const {
		return paramEditorOpenConnection;
	}
	//-------------------------------------------------------------------------
	parameter::Parameter::Connection & getParamEditorOpenConnection() {
		return paramEditorOpenConnection;
	}
	//-------------------------------------------------------------------------
	/**
	 * @return Editor-Pos-X Parameter
	 */
	processing::parameter::Parameter::Ptr getEditorPosX() const { return editorPosX; }
	//-------------------------------------------------------------------------
	/**
	 * @return Editor-Pos-Y Parameter
	 */
	processing::parameter::Parameter::Ptr getEditorPosY() const { return editorPosY; }
	//-------------------------------------------------------------------------
	/**
	 * @return Editor-Open/Close Parameter
	 */
	processing::parameter::Parameter::Ptr getEditorOpen() const { return editorOpen; }
	//-------------------------------------------------------------------------
	/**
	 * Editor-PosX Parameter Handler
	 * @param src
	 * @param val
	 */
	void paramEditorPosXChanged ( void *src, const float &val ) {
		com::events::EventSender<EditorPositionEvent>::notifyEventListeners (
			this,
			EditorPosition ( *editorPosX, *editorPosY )
		);
	}
	//-------------------------------------------------------------------------
	/**
	 * Editor-PosY Parameter Handler
	 * @param src
	 * @param val
	 */
	void paramEditorPosYChanged ( void *src, const float &val ) {
		com::events::EventSender<EditorPositionEvent>::notifyEventListeners ( 
			this,
			EditorPosition ( *editorPosX, *editorPosY )
		);
	}
	//-------------------------------------------------------------------------
	/**
	 * Editor-Open/Close Parameter Handler. Loest EditorOpenParameterChanged-Event aus.
	 * @param src
	 * @param val
	 */
	void paramEditorOpenChanged ( void *src, const float &val )
    {
		com::events::EventSender<EditorOpenParameterChanged>::notifyEventListeners ( 
			this,
			EditorOpenParameterChanged ( val > 0.5 )
		);
	}
	//-------------------------------------------------------------------------
	/**
	 * Editor-Open/Close Parameter Handler. Aktualisiert Parameter-Display.
	 * @param src
	 * @param val
	*/
	void paramEditorOpenDisplayChanged ( void *src, const float &val ) {
		bool onOpen = val > 0.5f;
		editorOpen->setDisplay ( onOpen ? "open" : "closed" );
	}
	//-------------------------------------------------------------------------
	/**
	 * @return Anzahl aller Plugin-Programme (aka. Presets)
	 */
	virtual size_t getNumPrograms() { return 0; }
	//-------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Program-Name zu index.
	 */
	virtual std::string getProgramName( size_t index ) { return ""; }
	//-------------------------------------------------------------------------
	/**
	 * Aktiviert Program zu index.
	 * @param index
	 */
	virtual void setProgram( size_t index ) {}
	//-------------------------------------------------------------------------
	/**
	 * @return index des akuell gesetzten Program, falls vorhanden. Andernfalls -1.
	 */
	virtual int getProgram() { return -1; }
	//-------------------------------------------------------------------------
	/**
	 * @return true, wenn von Client ausfuehrbar.
	 */
	virtual bool isAccessable() = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return Plugin-Uid.
	 */
	virtual int getUid() const { return pluginInfo.uid; }
	//-------------------------------------------------------------------------
	/**
	 * setzt Plugin-Uid
	 * @param uid
	 */
	virtual void setUid ( int uid ) { pluginInfo.uid = uid; }
	//-------------------------------------------------------------------------
	/**
	 * @return Plugin-Typ (@see PluginInfo::PluginType)
	 */
	virtual PluginInfo::PluginType getType() const { return pluginInfo.pluginType; }
	//-------------------------------------------------------------------------
	/**
	 * setzt Plugin-Typ (@see PluginInfo::PluginType)
	 * @param type
	 */
	virtual void setType( const PluginInfo::PluginType & type ) { pluginInfo.pluginType = type; }
	//-------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin == Synthesizer
	 */
	virtual bool isSynth() const { return pluginInfo.isSynth; }
	//-------------------------------------------------------------------------
	/**
	 * bestimmt ob Plugin == Synthesizer
	 * @param isSynth
	 */
	virtual void setIsSynth ( bool isSynth ) { pluginInfo.isSynth = isSynth; }
	//-------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin Midi-Event verarbeiten kann.
	 */
	virtual bool canHandleMidiEvent() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge des Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int numSamples ) = 0;
	//-------------------------------------------------------------------------
	/**
	 * Verarbeitet Midi-Events (@see VST-SDK VstEvents)
	 * @param events
	 */
	virtual void processMidiEvents( sambag::dsp::IMidiEvents * events ) = 0;
	//-------------------------------------------------------------------------
	virtual ~Plugin();
	//-------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin ueber Editor verfuegt.
	 */
	virtual bool hasEditor() const = 0;
	//-------------------------------------------------------------------------
	virtual void openEditor(void *window) {}
	//-------------------------------------------------------------------------
	virtual void closeEditor(void *window) {}
	//-------------------------------------------------------------------------
	virtual void onEditorIdle() {}
	//-------------------------------------------------------------------------
	/**
	 * @return Speicherort des Plugins
	 */
	std::string getLocation() const { return pluginInfo.location; }
	//-------------------------------------------------------------------------
	/**
	 * @return Pluginname
	 */
	com::MyString getPlugName() const { return pluginInfo.name; }
	//-------------------------------------------------------------------------
	/**
	 * @return Pluginhersteller
	 */
	com::MyString getPlugVendor() const { return plugVendor; }
	//-------------------------------------------------------------------------
	/**
	 * Setzt Pluginhersteller
	 * @param str
	 */
	void setPlugVendor( const com::MyString &str ) { plugVendor = str; }
	//-------------------------------------------------------------------------
	/**
	 * setzt Speicherort
	 * @param str
	 */
	void setLocation( const com::MyString &str ) { pluginInfo.location = str; }
	//-------------------------------------------------------------------------
	/**
	 * setzt Pluginname
	 * @param str
	 */
	void setPlugName( const com::MyString &str ) { pluginInfo.name = str; }
	//-------------------------------------------------------------------------
	/**
	 * @return PluginInfo zu Plugin.
	 */
	const PluginInfo & getPluginInfo() const { return pluginInfo; }
    //-------------------------------------------------------------------------
    /**
     * @brief get the number of input channels that the plugin is using.
     * @note don't confuse with getNumInputNodes because one inputnode uses
     * always two channels aka stereo.
     */
    virtual size_t getNumInputChannels() const = 0;
    //-------------------------------------------------------------------------
    /**
     * @brief get the number of output channels that the plugin is using.
     * @note don't confuse with getNumInputNodes because one outputnode uses
     * always two channels aka stereo.
     */

    virtual size_t getNumOutputChannels() const = 0;
};

//=============================================================================
/**
 * @class PluginFactory.
 * Erzeugt Plugin.
 * TODO: sollte erzeuger fuer alle Plugins sein(unabhaengig vom konkreten Typ).
 * Ist bisher nicht implementiert.
 */
class PluginFactory {
//=============================================================================
private:
	//-------------------------------------------------------------------------
	static Plugin::Ptr createVST2xPlugNode ( frx::processing::IHostInfo::Ptr hostInfo,
        const std::string &filename );
public:
	//-------------------------------------------------------------------------
	static Plugin::Ptr createPlugNode ( frx::processing::IHostInfo::Ptr hostInfo,
     const std::string &filename );
}; // pluginfactory
}// namespace processing

#endif


