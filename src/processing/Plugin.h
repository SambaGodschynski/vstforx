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
using namespace parameter;
//============================================================================================================
// Event: ResizeEditor.
//============================================================================================================
struct ResizeEditorEvent : public com::events::Event {
	size_t w, h;
	ResizeEditorEvent ( size_t width, size_t height ) : w(width), h(height) {}
};
//------------------------------------------------------------------------------------------------------------
typedef pair< float, float > EditorPosition; 
//------------------------------------------------------------------------------------------------------------
typedef com::events::ValueChangedEvent<EditorPosition> EditorPositionEvent;
//------------------------------------------------------------------------------------------------------------
struct EditorOpenParameterChanged : public com::events::Event {
	bool open;
	EditorOpenParameterChanged( bool open ) : open(open) {}
};
//============================================================================================================
// Klasse: Plugin.
// Oberklasse fuer Plugin.
//============================================================================================================
class Plugin: 
	public ProcessAdapter,
	public HasParameter,
	public MidiEventProcessor,
	public EventSender<EditorPositionEvent>,
	public EventSender<EditorOpenParameterChanged>,
	public com::events::EventSender<ResizeEditorEvent>
{
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<Plugin> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	PluginInfo pluginInfo;
	//--------------------------------------------------------------------------------------------------------
	string plugVendor;
	//--------------------------------------------------------------------------------------------------------
	// editor parameter:
	// They will be processed by GObjectController. To save their states independendly from view,
	// they are stored here and not in VSTPlugView.
	processing::parameter::Parameter::Ptr editorPosX, editorPosY, editorOpen;
	//--------------------------------------------------------------------------------------------------------
	template <typename Archive>
	void serialize ( Archive &ar, const unsigned int version ) {
		using namespace processing::parameter;
		ar & boost::serialization::base_object<ProcessAdapter> ( *this );
		ar & pluginInfo;
		ar & plugVendor;
		ar & editorPosX;
		ar & editorPosY;
		ar & editorOpen;
		if ( Archive::is_loading::value ) {
			// register editor pos parameter in plugin
			Parameter::ParameterListenerFunction xC = boost::bind( 
				&Plugin::paramEditorPosXChanged, this, _1, _2 
			);
			Parameter::ParameterListenerFunction yC = boost::bind( 
				&Plugin::paramEditorPosYChanged, this, _1, _2 
			);
			Parameter::ParameterListenerFunction oC = boost::bind( 
				&Plugin::paramEditorOpenChanged, this, _1, _2 
			);
			Parameter::ParameterListenerFunction dC = boost::bind( 
				&Plugin::paramEditorOpenDisplayChanged, this, _1, _2 
			);
			editorPosX->addValueChangedListenerF( xC );
			editorPosY->addValueChangedListenerF( yC );
			editorOpen->addValueChangedListenerF( oC );
			editorOpen->addValueChangedListenerF( dC );
		}
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	Plugin() {}
	//--------------------------------------------------------------------------------------------------------
	Plugin( IHostInfo *hostInfo, const string &location, size_t numInputs = 1, size_t numOutputs = 1 );
public:
	//--------------------------------------------------------------------------------------------------------
	processing::parameter::Parameter::Ptr getEditorPosX() const { return editorPosX; }
	//--------------------------------------------------------------------------------------------------------
	processing::parameter::Parameter::Ptr getEditorPosY() const { return editorPosY; }
	//--------------------------------------------------------------------------------------------------------
	processing::parameter::Parameter::Ptr getEditorOpen() const { return editorOpen; }
	//--------------------------------------------------------------------------------------------------------
	void paramEditorPosXChanged ( void *src, const float &val ) {
		EventSender<EditorPositionEvent>::notifyEventListeners ( 
			this,
			EditorPosition ( *editorPosX, *editorPosY )
		);
	}
	//--------------------------------------------------------------------------------------------------------
	void paramEditorPosYChanged ( void *src, const float &val ) {
		EventSender<EditorPositionEvent>::notifyEventListeners ( 
			this,
			EditorPosition ( *editorPosX, *editorPosY )
		);
	}
	//--------------------------------------------------------------------------------------------------------
	// two handler: paramEditorOpenChanged / paramEditorOpenDisplayChanged
	// because paramEditorOpenChanged will be skipped when editor not open/closed by this parameter. 
	void paramEditorOpenChanged ( void *src, const float &val ) {
		EventSender<EditorOpenParameterChanged>::notifyEventListeners ( 
			this,
			EditorOpenParameterChanged ( val > 0.5 )
		);
	}
	//--------------------------------------------------------------------------------------------------------
	void paramEditorOpenDisplayChanged ( void *src, const float &val ) {
		bool onOpen = val > 0.5f;
		editorOpen->setDisplay ( onOpen ? "open" : "closed" );
	}
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumPrograms() { return 0; }
	//--------------------------------------------------------------------------------------------------------
	virtual string getProgramName( size_t index ) { return ""; }
	//--------------------------------------------------------------------------------------------------------
	virtual void setProgram( size_t index ) {}
	//--------------------------------------------------------------------------------------------------------
	virtual int getProgram() { return -1; }
	//--------------------------------------------------------------------------------------------------------
	virtual bool isAccessable() = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual int getUid() const { return pluginInfo.uid; }
	//--------------------------------------------------------------------------------------------------------
	virtual void setUid ( int uid ) { pluginInfo.uid = uid; }
	//--------------------------------------------------------------------------------------------------------
	virtual PluginInfo::PluginType getType() const { return pluginInfo.pluginType; }
	//--------------------------------------------------------------------------------------------------------
	virtual void setType( const PluginInfo::PluginType & type ) { pluginInfo.pluginType = type; }
	//--------------------------------------------------------------------------------------------------------
	virtual bool isSynth() const { return pluginInfo.isSynth; }
	//--------------------------------------------------------------------------------------------------------
	virtual void setIsSynth ( bool isSynth ) { pluginInfo.isSynth = isSynth; }
	//--------------------------------------------------------------------------------------------------------
	virtual bool canHandleMidiEvent() const = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual void processAdapter( Processor::Int numSamples ) = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual void processMidiEvents( VstEvents * events ) = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual ~Plugin();
	//--------------------------------------------------------------------------------------------------------
	virtual bool hasEditor() const = 0;
	//--------------------------------------------------------------------------------------------------------
	string getLocation() const { return pluginInfo.location; }
	//--------------------------------------------------------------------------------------------------------
	MyString getPlugName() const { return pluginInfo.name; }
	//--------------------------------------------------------------------------------------------------------
	MyString getPlugVendor() const { return plugVendor; }
	//--------------------------------------------------------------------------------------------------------
	void setPlugVendor( const MyString &str ) { plugVendor = str; }
	//--------------------------------------------------------------------------------------------------------
	void setLocation( const MyString &str ) { pluginInfo.location = str; }
	//--------------------------------------------------------------------------------------------------------
	void setPlugName( const MyString &str ) { pluginInfo.name = str; }
	//--------------------------------------------------------------------------------------------------------
	const PluginInfo & getPluginInfo() const { return pluginInfo; }
};

//============================================================================================================
// Klasse: PluginFactory.
// erzeugt plugin.
//============================================================================================================
class PluginFactory {
private:
	//--------------------------------------------------------------------------------------------------------
	static Plugin::Ptr createVST2xPlugNode ( IHostInfo *hostInfo, const string &filename );
public:
	//--------------------------------------------------------------------------------------------------------
	static Plugin::Ptr createPlugNode ( IHostInfo *hostInfo, const string &filename );
}; // pluginfactory
}// namespace processing

#endif


