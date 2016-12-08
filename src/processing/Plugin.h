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
#include <sambag/com/events/PropertyChanged.hpp>
#include <sambag/com/events/Events.hpp>
#include <processing/IPlugin.hpp>

namespace frx { namespace processing {
namespace oldPr = ::processing;
namespace oldPrPr = ::processing::parameter;
namespace sce = sambag::com::events;
//=============================================================================
/**
 * Klasse: Plugin.
 * Oberklasse fuer Plugin.
 */
class Plugin:
	public oldPr::ProcessAdapter,
	public oldPr::parameter::HasParameter,
	public oldPr::MidiEventProcessor,
    public IPlugin
{
//=============================================================================
friend class boost::serialization::access;
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<Plugin> Ptr;
    //-------------------------------------------------------------------------
    static const std::string IO_CONFIG_CHANGED_MSG;
    //-------------------------------------------------------------------------
    static const std::string PROPERTY_PARAMETER_EDITOR_POSITION;
    //-------------------------------------------------------------------------
    static const std::string PROPERTY_PARAMETER_EDITOR_SIZE;
    //-------------------------------------------------------------------------
    static const std::string PROPERTY_PARAMETER_EDITOR_OPENSTATE;
private:
    //-------------------------------------------------------------------------
	bool processing;
	//-------------------------------------------------------------------------
	oldPrPr::Parameter::Connection paramEditorOpenConnection;
	//-------------------------------------------------------------------------
	oldPr::PluginInfo pluginInfo;
	//-------------------------------------------------------------------------
	std::string plugVendor;
	//-------------------------------------------------------------------------
	// editor parameter:
	// They will be processed by GObjectController. To save their states independendly from view,
	// they are stored here and not in VSTPlugView.
	oldPrPr::Parameter::Ptr editorPosX, editorPosY, editorOpen;
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
		ar & boost::serialization::base_object<oldPr::ProcessAdapter> ( *this );
		ar & boost::serialization::base_object<oldPr::MidiEventProcessor> ( *this );
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
            installListener();
		} else {
            saveImplState(ar, version, Int2Type<!IsLoading>());
        }
    }
    //-------------------------------------------------------------------------
    APluginImpl::Ptr impl;
    //-------------------------------------------------------------------------
	typedef std::vector<oldPr::Frames> Framebuffer;
	//-------------------------------------------------------------------------
	Framebuffer framebuffer;
    //-------------------------------------------------------------------------
	oldPr::Frames nullFrame; // fuer nicht genutzte eingaenge ( beim frame=>float[] )
	//-------------------------------------------------------------------------
	oldPr::Frames::T ** inMatrix;
	//-------------------------------------------------------------------------
	oldPr::Frames::T ** outMatrix;
    //-------------------------------------------------------------------------
    APluginImpl::Parameters parameters;
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
    void onImplMidiEvent(sambag::dsp::IMidiEvents::Ptr ev);
	//-------------------------------------------------------------------------
	/**
	 * Initalisiert Listener.
	 */
	void installListener();
	//-------------------------------------------------------------------------
	Plugin();
	//-------------------------------------------------------------------------
    Plugin( IHostInfo::Ptr hostInfo,
        const std::string &location,
        oldPr::PluginInfo::PluginType type
    );
public:
    //-------------------------------------------------------------------------
    /**
     * @brief stops plugin processing
     */
    void stopProcessing(const std::string &reason = "");
    //-------------------------------------------------------------------------
    /**
     * @brief resumes plugin processing
     */
    void resumeProcessing();
    //-------------------------------------------------------------------------
    APluginImpl::Ptr getPluginImpl() const {
        return impl;
    }
	//-------------------------------------------------------------------------
	virtual void setPresetData(const std::string &data) { impl->setPresetData(data); }
	//-------------------------------------------------------------------------
	virtual std::string getPresetData() { return impl->getPresetData(); }
    //-------------------------------------------------------------------------
    static Ptr create(IHostInfo::Ptr, const std::string &location);
    //-------------------------------------------------------------------------
    static Ptr createVST2x(IHostInfo::Ptr, const std::string &location);
    //-------------------------------------------------------------------------
    static Ptr createVST3x(IHostInfo::Ptr, const std::string &location);
    //-------------------------------------------------------------------------
    static Ptr createLua(IHostInfo::Ptr, const std::string &location);
    //-------------------------------------------------------------------------
    static Ptr createAU(IHostInfo::Ptr, const std::string &location);
    //-------------------------------------------------------------------------
    /**
     * @brief peek plugin informations
     */
    static void peek(IHostInfo::Ptr, oldPr::PluginInfo &info);
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
	const oldPrPr::Parameter::Connection & getParamEditorOpenConnection() const {
		return paramEditorOpenConnection;
	}
	//-------------------------------------------------------------------------
	oldPrPr::Parameter::Connection & getParamEditorOpenConnection() {
		return paramEditorOpenConnection;
	}
	//-------------------------------------------------------------------------
	/**
	 * @return Editor-Pos-X Parameter
	 */
	oldPrPr::Parameter::Ptr getEditorPosX() const { return editorPosX; }
	//-------------------------------------------------------------------------
	/**
	 * @return Editor-Pos-Y Parameter
	 */
	oldPrPr::Parameter::Ptr getEditorPosY() const { return editorPosY; }
	//-------------------------------------------------------------------------
	/**
	 * @return Editor-Open/Close Parameter
	 */
	oldPrPr::Parameter::Ptr getEditorOpen() const { return editorOpen; }
	//-------------------------------------------------------------------------
	/**
	 * Editor-PosX Parameter Handler
	 * @param src
	 * @param val
	 */
	void paramEditorPosXChanged ( void *src, const float &val );
	//-------------------------------------------------------------------------
	/**
	 * Editor-PosY Parameter Handler
	 * @param src
	 * @param val
	 */
	void paramEditorPosYChanged ( void *src, const float &val );
	//-------------------------------------------------------------------------
	/**
	 * Editor-Open/Close Parameter Handler. Loest EditorOpenParameterChanged-Event aus.
	 * @param src
	 * @param val
	 */
	void paramEditorOpenChanged ( void *src, const float &val );
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
	std::string getUid() const { return pluginInfo.uid; }
	//-------------------------------------------------------------------------
	/**
	 * setzt Plugin-Uid
	 * @param uid
	 */
	void setUid ( std::string uid ) { pluginInfo.uid = uid; }
	//-------------------------------------------------------------------------
	/**
	 * @return Plugin-Typ (@see PluginInfo::PluginType)
	 */
	oldPr::PluginInfo::PluginType getType() const { return pluginInfo.pluginType; }
	//-------------------------------------------------------------------------
	/**
	 * setzt Plugin-Typ (@see PluginInfo::PluginType)
	 * @param type
	 */
	void setType( const oldPr::PluginInfo::PluginType & type ) { pluginInfo.pluginType = type; }
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
    virtual void beforeOpenEditor(sambag::disco::components::WindowPtr win);
	//-------------------------------------------------------------------------
	void openEditor(sambag::disco::components::WindowPtr);
	//-------------------------------------------------------------------------
	void closeEditor(sambag::disco::components::WindowPtr);
	//-------------------------------------------------------------------------
	void onEditorIdle();
	//-------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge des Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	void processAdapter( oldPr::Processor::Int numSamples );
	//-------------------------------------------------------------------------
	/**
	 * Verarbeitet Midi-Events (@see VST-SDK VstEvents)
	 * @param events
	 */
	void processMidiEvents( sambag::dsp::IMidiEvents::Ptr events );
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
	std::string getPlugName() const { return pluginInfo.name; }
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
	const oldPr::PluginInfo & getPluginInfo() const { return pluginInfo; }
	//-------------------------------------------------------------------------
	/**
	 * Host-Info changed Handler
	 */
	void hostBaseConfigChanged();
    //-------------------------------------------------------------------------
    oldPrPr::Parameter::Ptr getParameter (size_t nr=0) const {
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
    const bool LuaPluginReg =
        ::frx::processing::ModelFactory::instance().registerWithDetail(
                "lua.Plugin", &Plugin::createLua
    );
}
}}// namespace processing

#endif


