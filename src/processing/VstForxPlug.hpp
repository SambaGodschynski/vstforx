/*
 * VstForxPlug.hpp
 *
 *  Created on: Mon Oct  8 12:58:07 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_VSTFORXPLUG_H
#define SAMBAG_VSTFORXPLUG_H

#include <boost/shared_ptr.hpp>
#include <sambag/com/events/Events.hpp>
#include <sambag/com/ArithmeticWrapper.hpp>
#include <sambag/dsp/DspPlugin.hpp>
#include <processing/IHostInfo.h>
#include <processing/Graph.h>
#include <processing/ModelController.hpp>
#include <gui/ViewModelMap.hpp>
#include <gui/components/Forward.hpp>
#include <iostream>
#include <com/Serialization.h>
#include <sambag/com/Thread.hpp>
#include <com/FrxConfig.h>
#include <scripts/PluginScriptCtrl.hpp>
#include <list>
#include <processing/dspTools.h>

extern const char * globGetProductName();

namespace frx { namespace processing {
namespace sce=sambag::com::events;
namespace fg = frx::gui;
namespace fgc = fg::components;
//=============================================================================
class VstForxPlug :
    public sambag::dsp::PluginProcessorBase,
	public IHostInfo,
	public sce::EventSender<HostIOChanged>
{
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef sambag::dsp::PluginProcessorBase Super;
	//-------------------------------------------------------------------------
	typedef ::frx::scripts::PluginScriptCtrl ScriptCtrl;
    typedef boost::shared_ptr<ScriptCtrl> ScriptCtrlPtr;
private:
    //-------------------------------------------------------------------------
    ScriptCtrlPtr scriptCtrl;
	//-------------------------------------------------------------------------
	void *effectPtr;
	//-------------------------------------------------------------------------
	void *masterCallback;
	//-------------------------------------------------------------------------
	int blockSize;
	//-------------------------------------------------------------------------
	char *chunkData;
	//-------------------------------------------------------------------------
	float sampleRate;
	//-------------------------------------------------------------------------
	sambag::com::ArithmeticWrapper<bool> _open;
	//-------------------------------------------------------------------------
	// we can't create a VstForxPlug instance as shared_ptr 
	// so we need a adapter.
	IHostInfo::Ptr hostInfoAdapter;
	//-------------------------------------------------------------------------
	::processing::Graph::Ptr graph;
	//-------------------------------------------------------------------------
	ModelController::Ptr ctrl;
	//-------------------------------------------------------------------------
	frx::gui::ViewModelMap::Ptr map;
	//-------------------------------------------------------------------------
	sambag::com::ArithmeticWrapper<bool> onHostParameterUpdate;
	//-------------------------------------------------------------------------
	void save(std::ostream &os);
	//-------------------------------------------------------------------------
	void load(std::istream &is, int version = FRX_ARCHIVE_VERSION);
	//-------------------------------------------------------------------------
	void saveEditor(::com::oArchive &ar);
	//-------------------------------------------------------------------------
	void loadEditor(::com::iArchive &ar, int version = FRX_ARCHIVE_VERSION);
	//-------------------------------------------------------------------------
    /**
     * some hosts(ableton live) give no timeInfo when the requesting thread is 
     * another than the plugin thread.
     */
    sambag::dsp::HostTimeInfo lastTimeInfo;
    sambag::com::ThreadId processingThread;
    //-------------------------------------------------------------------------
    sambag::com::Mutex processingLoadLock;
protected:
    //-------------------------------------------------------------------------
    void processTasks();
    //-------------------------------------------------------------------------
    void onScriptExeFailed(const frx::scripts::ScriptExeFailedEvent &ev);
	//-------------------------------------------------------------------------
	void installGraphListener();
	//-------------------------------------------------------------------------
	void onGraphDelayChanged(void *src, const ::processing::GraphDelayChanged &ev);
	//-------------------------------------------------------------------------
	/**
	 * updates graph samplerate and blocksize
	 */
	void updateGraphBaseConfiguration();
	//-------------------------------------------------------------------------
	void setBlockSize(int blockSize);
	//-------------------------------------------------------------------------
	void setSampleRate(float blockSize);
	//-------------------------------------------------------------------------
	void setParameterValue(int index, float value);
	//-------------------------------------------------------------------------
	void getParameterValue(int index, float &outValue);
	//-------------------------------------------------------------------------
	void getParameterName (int index, std::string &outStr) const;
	//-------------------------------------------------------------------------
	void unRegisterInstance();
	//-------------------------------------------------------------------------
	void initHostParameter();
    //-------------------------------------------------------------------------
    void updateLegacy(::processing::ProcessAdapterPtr old,
        ::processing::ProcessAdapterPtr _new);
public:
	//-------------------------------------------------------------------------
	bool isOpen() const { return _open; }
	//-------------------------------------------------------------------------
	void open();
	//-------------------------------------------------------------------------
	void close();
	//-------------------------------------------------------------------------
	void * getEditor();
	//-------------------------------------------------------------------------
	bool requestEditorResize(int width, int height);
	//-------------------------------------------------------------------------
	void processEvents(sambag::dsp::IMidiEvents *ev);
	//-------------------------------------------------------------------------
	void hostParameterChanged(void *src, float value, int index);
	//-------------------------------------------------------------------------
	void registerView(fgc::FrxCircuidViewPtr view);
	//-------------------------------------------------------------------------
	void unRegisterView(fgc::FrxCircuidViewPtr view);
	//-------------------------------------------------------------------------
	ModelController::Ptr getModelController() const {
		return ctrl;
	}
	//-------------------------------------------------------------------------
	::processing::Graph::Ptr getGraph() const {
		return graph;
	}
	//-------------------------------------------------------------------------
	frx::gui::ViewModelMap::Ptr getViewModelMap() const {
		return map;
	}
    //-------------------------------------------------------------------------
    ScriptCtrlPtr getScriptCtrl();
    //-------------------------------------------------------------------------
	void setEffectPtr(void *effPtr);
	//-------------------------------------------------------------------------
	void setMasterCallback(void *mCallbk);
	//-------------------------------------------------------------------------
	~VstForxPlug();
	//-------------------------------------------------------------------------
	VstForxPlug();
	//-------------------------------------------------------------------------
	void process(float **in, float**out, int numSamples);
	//-------------------------------------------------------------------------
	///////////////////
	// IHostInfo Impl.
	///////////////////
    //-------------------------------------------------------------------------
    virtual scripts::PluginScriptCtrlPtr getScriptController() const {
        return scriptCtrl;
    }
	//-------------------------------------------------------------------------
	virtual float getSampleRate() const;
	//-------------------------------------------------------------------------
	virtual int getBlockSize() const;
	//-------------------------------------------------------------------------
	/**
	 * something changed, update needed.
	 * @return value is given by vst-sdk but not documented. So I don't know what
	 *         the return value means.
	 */
	virtual bool ioChanged();
	//-------------------------------------------------------------------------
	virtual TimeInfo * getHostTimeInfo (int filter);
	//-------------------------------------------------------------------------
	virtual HostIOChangedConnection 
	addHostChangedListener(const HostIOChangedFunction &f);
	//-------------------------------------------------------------------------
	virtual HostIOChangedConnection 
	addTrackedHostChangedListener(const HostIOChangedFunction &f, AnyWPtr wptr);
	//-------------------------------------------------------------------------
	virtual void * getEffectPtr();
	//-------------------------------------------------------------------------
	virtual void * getMasterCallback();
	//-------------------------------------------------------------------------
	int getChunk(void **data);
	//-------------------------------------------------------------------------
	int setChunk(void *data, int byteSize, int version = FRX_ARCHIVE_VERSION);
	//-------------------------------------------------------------------------
	int getLatency() const;
	//-------------------------------------------------------------------------
	template <class String> 
	void getVendor(String &outStr) const {
		outStr="www.vstforx.de";
	}
	//-------------------------------------------------------------------------
	template <class String> 
	void getProductName(String &outStr) const {
		outStr = std::string( globGetProductName() );
	} 
	//-------------------------------------------------------------------------
	int getProductVersion() const { 
		return FRX_VERSION_MAJOR*1000 + FRX_VERSION_MINOR*100 + FRX_VERSION_MICRO;
	}
    //-------------------------------------------------------------------------
    virtual MasterType getMasterType() const {
        return VST2X;
    }
};
}} // namespace

#endif /* SAMBAG_VSTFORXPLUG_H */
