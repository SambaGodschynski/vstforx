#ifndef PPICORE_PPIVST_H
#define PPICORE_PPIVST_H

/*
	==============================================================
	*			      	VSTForx-Windows 						 *
	==============================================================
*/
#define MEMORY_TRACKING_STD 
// memory tracking :
#ifdef MEMORY_TRACKING_STD
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#ifdef MEMORY_TRACKING_VLD
#include "vld.h" // for detailed leak info
#endif

#include "AudioEffectX.h"
#include "gui/PpiEditor.h"
#include "processing/parameter/parameter.h"
#include "processing/processing.h"
#include "resource.h"
#include "com/PluginCollection.h"
#include "OS_Specific/OS_gui.h"
#include <gdiplus.h> 
#include "com/Command.h"
#include "processing/IHostInfo.h"

using namespace std;
using namespace com;
using namespace processing;
using namespace parameter;
//===========================================================================================
/**
 * @class PPIVst.
 * AudioEffectX class for VSTForx.
 * TODO: create one classfile for mac and win.
 * Problems trying migrate to VSTPluginWrapper:
 *   - Editor needs graph on open
 *   - IHostInfo where to get?
 */
class PPIVst :
	public AudioEffectX, 
	public EventListener<ppiGui::OnClose>,
	public EventListener<GraphChanged>,
	public IHostInfo
//===========================================================================================
{
private:
	//---------------------------------------------------------------------------------------
	float sampleRate;
	//---------------------------------------------------------------------------------------
	int blockSize;
	//---------------------------------------------------------------------------------------
	char *chunk;
	//---------------------------------------------------------------------------------------
	int oldDelay;
	//---------------------------------------------------------------------------------------
	//Der Graph der die Prozesskette repraesentiert
	Graph::Ptr graph;
	//---------------------------------------------------------------------------------------
	com::PluginCollection::Ptr pluginCollection;
	//---------------------------------------------------------------------------------------
	static int instances;
	//---------------------------------------------------------------------------------------
	void eventHandler ( void *src, const ppiGui::OnClose &ev );
	//---------------------------------------------------------------------------------------
	void eventHandler ( void *src, const GraphChanged &ev ) {
		int delay = graph->getGraphDelay();
		if ( oldDelay == delay ) return;
		setInitialDelay( delay );
		oldDelay = delay;
		AudioEffectX::ioChanged();
	}
	//---------------------------------------------------------------------------------------
	void initHostParameter();
	//---------------------------------------------------------------------------------------
	bool onUpdate;
	//---------------------------------------------------------------------------------------
	Settings::Ptr settings;
	//---------------------------------------------------------------------------------------
	com::CommandWorker::Ptr commandWorker;
	//---------------------------------------------------------------------------------------
	ULONG_PTR gdiplusToken; 
public:
	//---------------------------------------------------------------------------------------
	virtual void processReplacing(float ** inputs, float ** outputs, VstInt32 numSamples);
	//---------------------------------------------------------------------------------------
	virtual void setParameter(VstInt32 index, float value);
    //---------------------------------------------------------------------------------------
	virtual float getParameter(VstInt32 index);
	//---------------------------------------------------------------------------------------
	virtual void open ();
	//---------------------------------------------------------------------------------------
	virtual void close ();
	//---------------------------------------------------------------------------------------
	virtual VstInt32 getNumMidiInputChannels() { return 16; }
	//---------------------------------------------------------------------------------------
	virtual void setSampleRate( float sr );
	//---------------------------------------------------------------------------------------
	virtual void setBlockSize ( VstInt32 bs );
	//---------------------------------------------------------------------------------------
	virtual VstInt32 canDo ( char *text );
    //---------------------------------------------------------------------------------------
	virtual void getParameterLabel(VstInt32 index, char * label);
    //---------------------------------------------------------------------------------------
	virtual void getParameterDisplay(VstInt32 index, char * text);
	//---------------------------------------------------------------------------------------
	virtual void getParameterName(VstInt32 index, char * name);
	//---------------------------------------------------------------------------------------
	virtual void valueChanged ( void *src, const float &val );
    //---------------------------------------------------------------------------------------
	virtual VstInt32 processEvents(VstEvents * events);
	//---------------------------------------------------------------------------------------
	virtual bool getEffectName(char *name) {
		vst_strncpy ( name , Settings::NAME.c_str(), Settings::NAME.length() );
		return true;
	}
	//---------------------------------------------------------------------------------------
	virtual bool getVendorString(char *text) {
		vst_strncpy ( text , Settings::NAME.c_str(), Settings::VENDOR.length() );
		return true;
	}
 	//---------------------------------------------------------------------------------------
	virtual VstInt32 getVendorVersion() {
		return Settings::PPI_VERSION;
	}
    //---------------------------------------------------------------------------------------
	//wird aufgerufen wenn Host Parameter Chunk abruft.
    //liefert groesse des Chunks in byte.
    virtual VstInt32 getChunk(void ** data, bool isPreset);
    //---------------------------------------------------------------------------------------
	virtual VstInt32 setChunk(void * data, VstInt32 byteSize, bool isPreset);
    //---------------------------------------------------------------------------------------
	PPIVst(audioMasterCallback audioMaster);
	//---------------------------------------------------------------------------------------
	virtual void suspend();
	//---------------------------------------------------------------------------------------
	virtual void resume();
    //---------------------------------------------------------------------------------------
	virtual ~PPIVst();
	//---------------------------------------------------------------------------------------
	virtual float getSampleRate() const { return sampleRate; }
	//---------------------------------------------------------------------------------------
	virtual int getBlockSize() const { return blockSize; }
	//---------------------------------------------------------------------------------------
	virtual VstTimeInfo * getVstTimeInfo ( VstInt32 filter ) { return getTimeInfo(filter); }
	//---------------------------------------------------------------------------------------
	virtual AudioEffectX * getAudioEffectX() { return this; }
	//---------------------------------------------------------------------------------------
	virtual AudioMasterCallback getAudioMasterCallback() { return audioMaster; }
	//---------------------------------------------------------------------------------------
	virtual bool IHostInfo::ioChanged() { return true; }
};

#endif
