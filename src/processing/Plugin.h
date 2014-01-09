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
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include "pluginTypes/PluginImpl.hpp"
#include <processing/ModelFactory.hpp>

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
	bool processing;
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
    void restorePluginInfo();
    //-------------------------------------------------------------------------
    void loadImpl();
    //-------------------------------------------------------------------------
    template<int I> struct Int2Type { enum {Value = I}; };
	//-------------------------------------------------------------------------
    void saveImplState(com::oArchive &ar, const unsigned int version, Int2Type<1>);
	//-------------------------------------------------------------------------
    void loadImplState(com::iArchive &ar, const unsigned int version, Int2Type<1>);
	//-------------------------------------------------------------------------
    template <class Archive>
    void saveImplState(Archive&, const unsigned int , Int2Type<0>){}
	//-------------------------------------------------------------------------
    template <class Archive>
    void loadImplState(Archive&, const unsigned int , Int2Type<0>){}
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
        ar & pluginInfo;
        ar & parameters;
        enum { IsLoading = Archive::is_loading::value };
		if ( IsLoading ) {
			loadImplState(ar, version, Int2Type<IsLoading>());
            impl->updatePluginInfo(pluginInfo);
            initListener();
		} else {
            saveImplState(ar, version, Int2Type<!IsLoading>());
        }
    }
    //-------------------------------------------------------------------------
    frx::processing::APluginImpl *impl;
    //-------------------------------------------------------------------------
	typedef std::vector<Frames> Framebuffer;
	//-------------------------------------------------------------------------
	Framebuffer framebuffer;
    //-------------------------------------------------------------------------
	Frames nullFrame; // fuer nicht genutzte eingaenge ( beim frame=>float[] )
	//-------------------------------------------------------------------------
	float ** inMatrix;
	//-------------------------------------------------------------------------
	float ** outMatrix;
    //-------------------------------------------------------------------------
    frx::processing::APluginImpl::Parameters parameters;
	//-------------------------------------------------------------------------
	/**
	 * INITALISIERT Framesbuffer
	 */
	void setupFramesbuffer();
    //-------------------------------------------------------------------------
    void onImplPropertyChanged(void*,
        const sambag::com::events::PropertyChanged &ev);
protected:
	//-------------------------------------------------------------------------
	/**
	 * Initalisiert Listener.
	 */
	void initListener();
	//-------------------------------------------------------------------------
	Plugin();
	//-------------------------------------------------------------------------
    Plugin( frx::processing::IHostInfo::Ptr hostInfo,
        const std::string &location,
        PluginInfo::PluginType type
    );
public:
    //-------------------------------------------------------------------------
    static Ptr create(frx::processing::IHostInfo::Ptr, const std::string &location);
    //-------------------------------------------------------------------------
    static Ptr createVST2x(frx::processing::IHostInfo::Ptr, const std::string &location);
    //-------------------------------------------------------------------------
    static Ptr createVST3x(frx::processing::IHostInfo::Ptr, const std::string &location);
    //-------------------------------------------------------------------------
    static Ptr createAU(frx::processing::IHostInfo::Ptr, const std::string &location);
    //-------------------------------------------------------------------------
    /**
     * @override
     */
    virtual std::string getStatusMessage() const;
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
	 * @return true, wenn von Client ausfuehrbar.
	 */
	bool isAccessable() const;
	//-------------------------------------------------------------------------
	/**
	 * @return Plugin-Uid.
	 */
	int getUid() const { return pluginInfo.uid; }
	//-------------------------------------------------------------------------
	/**
	 * setzt Plugin-Uid
	 * @param uid
	 */
	void setUid ( int uid ) { pluginInfo.uid = uid; }
	//-------------------------------------------------------------------------
	/**
	 * @return Plugin-Typ (@see PluginInfo::PluginType)
	 */
	PluginInfo::PluginType getType() const { return pluginInfo.pluginType; }
	//-------------------------------------------------------------------------
	/**
	 * setzt Plugin-Typ (@see PluginInfo::PluginType)
	 * @param type
	 */
	void setType( const PluginInfo::PluginType & type ) { pluginInfo.pluginType = type; }
	//-------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin == Synthesizer
	 */
	bool isSynth() const { return pluginInfo.isSynth; }
	//-------------------------------------------------------------------------
	/**
	 * bestimmt ob Plugin == Synthesizer
	 * @param isSynth
	 */
	void setIsSynth ( bool isSynth ) { pluginInfo.isSynth = isSynth; }
	//-------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin Midi-Event verarbeiten kann.
	 */
	bool canHandleMidiEvent() const;
    //-------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin ueber Editor verfuegt.
	 */
	bool hasEditor() const;
	//-------------------------------------------------------------------------
	void openEditor(void *window);
	//-------------------------------------------------------------------------
	void closeEditor(void *window);
	//-------------------------------------------------------------------------
	void onEditorIdle();
	//-------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge des Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	void processAdapter( Processor::Int numSamples );
	//-------------------------------------------------------------------------
	/**
	 * Verarbeitet Midi-Events (@see VST-SDK VstEvents)
	 * @param events
	 */
	void processMidiEvents( sambag::dsp::IMidiEvents * events );
	//-------------------------------------------------------------------------
	virtual ~Plugin();
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
	 * Host-Info changed Handler
	 */
	void hostBaseConfigChanged();
    //-------------------------------------------------------------------------
    parameter::ParameterPtr getParameter (size_t nr=0) const {
        return parameters.at(nr);
    }
    //-------------------------------------------------------------------------
    size_t 	getNumParameter () const {
        return parameters.size();
    }
    //-------------------------------------------------------------------------
	/**
	 * @return Anzahl aller Plugin-Programme (aka. Presets)
	 */
	size_t getNumPrograms();
	//-------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Program-Name zu index.
	 */
	std::string getProgramName( size_t index );
	//-------------------------------------------------------------------------
	/**
	 * Aktiviert Program zu index.
	 * @param index
	 */
	void setProgram( size_t index );
	//-------------------------------------------------------------------------
	/**
	 * @return index des akuell gesetzten Program, falls vorhanden. Andernfalls -1.
	 */
	int getProgram();
    //-------------------------------------------------------------------------
    size_t getNumInputChannels() const;
    //-------------------------------------------------------------------------
    size_t getNumOutputChannels() const;
};

namespace {
    const bool UnkownPluginReg =
        ::frx::processing::ModelFactory::instance().registerWithDetail<Plugin>(
                "unknown-plugin.Plugin", &Plugin::create
    );
    const bool VST2xPluginReg =
        ::frx::processing::ModelFactory::instance().registerWithDetail(
                "vst2x.Plugin", &Plugin::createVST2x
    );
    const bool VST3PluginReg =
        ::frx::processing::ModelFactory::instance().registerWithDetail(
                "vst3x.Plugin", &Plugin::createVST3x
    );
    const bool AUPluginReg =
        ::frx::processing::ModelFactory::instance().registerWithDetail(
                "au.Plugin", &Plugin::createAU
    );
}
}// namespace processing

#endif


