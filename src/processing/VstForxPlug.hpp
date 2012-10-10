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
#include <sambag/dsp/DspPlugin.hpp>
#include <processing/IHostInfo.h>
#include <processing/graph.h>
#include <processing/ModelController.hpp>
#include <gui/ViewModelMap.hpp>
#include <gui/components/Forward.hpp>

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
private:
	//-------------------------------------------------------------------------
	void *effectPtr;
	//-------------------------------------------------------------------------
	void *masterCallback;
	//-------------------------------------------------------------------------
	int blockSize;
	//-------------------------------------------------------------------------
	float sampleRate;
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
	void open();
	//-------------------------------------------------------------------------
	void close();
	//-------------------------------------------------------------------------
	void unRegisterInstance();
public:
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
};
}} // namespace

#endif /* SAMBAG_VSTFORXPLUG_H */
