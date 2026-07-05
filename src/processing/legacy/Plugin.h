/*
 * ===========================================================================================================
 * Plugin.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FRX_LEGACY_PLUGNODE_H
#define FRX_LEGACY_PLUGNODE_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/one4All.h"
#include "com/SerializationFwd.h"
#include "com/Events.h"
#include <processing/PlugInfo.h>
#include "processing/MidiEventProcessor.h"
#include <sambag/com/events/PropertyChanged.hpp>
#include <sambag/com/events/Events.hpp>
#include <processing/IPlugin.hpp>

namespace frx { namespace processing {
    struct APluginImpl;
    typedef std::shared_ptr<APluginImpl> APluginImplPtr;
}}

namespace processing {
namespace sce = sambag::com::events;
//============================================================================================================
/**
 * Klasse: Plugin.
 * Oberklasse fuer Plugin.
 */
class Plugin : public ::processing::ProcessAdapter,
	public ::processing::parameter::HasParameter,
	public ::processing::MidiEventProcessor,
    public frx::processing::IPlugin
{
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef std::shared_ptr<Plugin> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	std::string statusMsg;
	//--------------------------------------------------------------------------------------------------------
	::processing::parameter::Parameter::Connection paramEditorOpenConnection;
	//--------------------------------------------------------------------------------------------------------
	::processing::PluginInfo pluginInfo;
	//--------------------------------------------------------------------------------------------------------
	std::string plugVendor;
	//--------------------------------------------------------------------------------------------------------
	// editor parameter:
	// They will be processed by GObjectController. To save their states independendly from view,
	// they are stored here and not in VSTPlugView.
	::processing::parameter::Parameter::Ptr editorPosX, editorPosY, editorOpen;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert Plugin-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template <typename Archive>
	void serialize ( Archive &ar, const unsigned int version ) {
		using namespace ::processing::parameter;
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
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Initalisiert Listener.
	 */
	void initListener();
	//--------------------------------------------------------------------------------------------------------
	Plugin() {}
	//--------------------------------------------------------------------------------------------------------
	Plugin( frx::processing::IHostInfo::Ptr hostInfo, const std::string &location, size_t numInputs = 1, size_t numOutputs = 1 );
public:
    //--------------------------------------------------------------------------------------------------------
    /**
     * @override
     */
    virtual std::string getStatusMessage() const { return statusMsg; }
	//--------------------------------------------------------------------------------------------------------
	void setStatusMsg( const std::string &msg );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Wird von GPluginController benoetigt um bei bedarf verbindung zu blockieren.
	 * TODO: schlechte Loesung!
	 * @return Parameter-EditorOpenChanged-Connection
	 */
	const ::processing::parameter::Parameter::Connection & getParamEditorOpenConnection() const {
		return paramEditorOpenConnection;
	}
	//--------------------------------------------------------------------------------------------------------
	::processing::parameter::Parameter::Connection & getParamEditorOpenConnection() {
		return paramEditorOpenConnection;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Editor-Pos-X Parameter
	 */
	::processing::parameter::Parameter::Ptr getEditorPosX() const { return editorPosX; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Editor-Pos-Y Parameter
	 */
	::processing::parameter::Parameter::Ptr getEditorPosY() const { return editorPosY; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Editor-Open/Close Parameter
	 */
	::processing::parameter::Parameter::Ptr getEditorOpen() const { return editorOpen; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Editor-PosX Parameter Handler
	 * @param src
	 * @param val
	 */
	void paramEditorPosXChanged ( void *src, const float &val );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Editor-PosY Parameter Handler
	 * @param src
	 * @param val
	 */
	void paramEditorPosYChanged ( void *src, const float &val );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Editor-Open/Close Parameter Handler. Loest EditorOpenParameterChanged-Event aus.
	 * @param src
	 * @param val
	 */
	void paramEditorOpenChanged ( void *src, const float &val );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Editor-Open/Close Parameter Handler. Aktualisiert Parameter-Display.
	 * @param src
	 * @param val
	*/
	void paramEditorOpenDisplayChanged ( void *src, const float &val ) {
		bool onOpen = val > 0.5f;
		editorOpen->setDisplay ( onOpen ? "open" : "closed" );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller Plugin-Programme (aka. Presets)
	 */
	virtual size_t getNumPrograms() { return 0; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Program-Name zu index.
	 */
	virtual std::string getProgramName( size_t index ) { return ""; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Aktiviert Program zu index.
	 * @param index
	 */
	virtual void setProgram( size_t index ) {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return index des akuell gesetzten Program, falls vorhanden. Andernfalls -1.
	 */
	virtual int getProgram() { return -1; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn von Client ausfuehrbar.
	 */
	virtual bool isAccessable() = 0;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Plugin-Uid.
	 */
	virtual std::string getUid() const { return pluginInfo.uid; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Plugin-Uid
	 * @param uid
	 */
	virtual void setUid ( std::string uid ) { pluginInfo.uid = uid; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Plugin-Typ (@see PluginInfo::PluginType)
	 */
	virtual ::processing::PluginInfo::PluginType getType() const { return pluginInfo.pluginType; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Plugin-Typ (@see PluginInfo::PluginType)
	 * @param type
	 */
	virtual void setType( const ::processing::PluginInfo::PluginType & type ) { pluginInfo.pluginType = type; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin == Synthesizer
	 */
	virtual bool isSynth() const { return pluginInfo.isSynth; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * bestimmt ob Plugin == Synthesizer
	 * @param isSynth
	 */
	virtual void setIsSynth ( bool isSynth ) { pluginInfo.isSynth = isSynth; }
	//----------[A----------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin Midi-Event verarbeiten kann.
	 */
	virtual bool canHandleMidiEvent() const = 0;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge des Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processAdapter( ::processing::Processor::Int numSamples ) = 0;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Midi-Events (@see VST-SDK VstEvents)
	 * @param events
	 */
	virtual void processMidiEvents( sambag::dsp::IMidiEvents::Ptr events ) = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual ~Plugin();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin ueber Editor verfuegt.
	 */
	virtual bool hasEditor() const = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual void openEditor(sambag::disco::components::WindowPtr win) {}
	//--------------------------------------------------------------------------------------------------------
	virtual void closeEditor(sambag::disco::components::WindowPtr win) {}
	//--------------------------------------------------------------------------------------------------------
	virtual void onEditorIdle() {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Speicherort des Plugins
	 */
	std::string getLocation() const { return pluginInfo.location; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Pluginname
	 */
	std::string getPlugName() const { return pluginInfo.name; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Pluginhersteller
	 */
	std::string getPlugVendor() const { return plugVendor; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Setzt Pluginhersteller
	 * @param str
	 */
	void setPlugVendor( const com::MyString &str ) { plugVendor = str; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Speicherort
	 * @param str
	 */
	void setLocation( const com::MyString &str ) { pluginInfo.location = str; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Pluginname
	 * @param str
	 */
	void setPlugName( const com::MyString &str ) { pluginInfo.name = str; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return PluginInfo zu Plugin.
	 */
	const ::processing::PluginInfo & getPluginInfo() const { return pluginInfo; }
    //--------------------------------------------------------------------------------------------------------
    virtual frx::processing::APluginImplPtr getPluginImpl() const {
        return frx::processing::APluginImplPtr();
    }
	//-------------------------------------------------------------------------
	virtual void setPresetData(const std::string &data) = 0;
	//-------------------------------------------------------------------------
	virtual std::string getPresetData() = 0;
};
}

#endif
