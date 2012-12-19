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
#include <processing/graph.h>
#include <processing/ModelController.hpp>
#include <gui/ViewModelMap.hpp>
#include <gui/components/Forward.hpp>
#include <iostream>
#include <com/Serialization.h>

namespace frx { namespace processing {
namespace sce=sambag::com::events;
namespace fg = frx::gui;
namespace fgc = fg::components;
//=============================================================================
class VstForxPlug : public sambag::dsp::PluginProcessorBase,
	public IHostInfo,
	public sce::EventSender<HostIOChanged>
{
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef sambag::dsp::PluginProcessorBase Super;
private:
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
	void load(std::istream &is);
	//-------------------------------------------------------------------------
	void saveEditor(::com::oArchive &ar);
	//-------------------------------------------------------------------------
	void loadEditor(::com::iArchive &ar);
protected:
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
	void unRegisterInstance();
	//-------------------------------------------------------------------------
	void initHostParameter();
public:
	//-------------------------------------------------------------------------
	bool isOpen() const { return _open; }
	//-------------------------------------------------------------------------
	void open();
	//-------------------------------------------------------------------------
	void close();
	//-------------------------------------------------------------------------
	sambag::dsp::IEditor * getEditor();
	//-------------------------------------------------------------------------
	void requestEditorResize(int width, int height);
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
	int setChunk(void *data, int byteSize);
};
}} // namespace

#endif /* SAMBAG_VSTFORXPLUG_H */
