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

namespace frx { namespace processing {
namespace sce=sambag::com::events;
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
public:
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
