/*
 * VstForxPlug.cpp
 *
 *  Created on: Mon Oct  8 12:58:07 2012
 *      Author: Johannes Unger
 */

#include "VstForxPlug.hpp"
#include <processing/IModelController.hpp>
#include <stdlib.h>
#include <sstream>
#include <processing/Frames.h>
#include <com/one4All.h>

namespace frx { namespace processing {
namespace {
	struct HostInfoAdapter : public IHostInfo {
		IHostInfo &hostInfo;
		HostInfoAdapter(IHostInfo &hostInfo) : hostInfo(hostInfo) {}
		virtual float getSampleRate() const {
			return hostInfo.getSampleRate();
		}
		virtual int getBlockSize() const {
			return hostInfo.getBlockSize();
		}
		virtual bool ioChanged() {
			return hostInfo.ioChanged();
		}
		virtual TimeInfo * getHostTimeInfo (int filter) {
			return hostInfo.getHostTimeInfo(filter);
		}
		virtual HostIOChangedConnection 
			addHostChangedListener(const HostIOChangedFunction &f) 
		{
			return hostInfo.addHostChangedListener(f);
		}
		virtual HostIOChangedConnection 
		addTrackedHostChangedListener(const HostIOChangedFunction &f, AnyWPtr wptr)
		{
			return hostInfo.addTrackedHostChangedListener(f, wptr);
		}
		virtual void * getEffectPtr() {
			return hostInfo.getEffectPtr();
		}
		virtual void * getMasterCallback() {
			return hostInfo.getMasterCallback();
		}	
	};
} // namespace
//=============================================================================
// class VstForxPlug 
//=============================================================================
//-----------------------------------------------------------------------------
VstForxPlug::VstForxPlug() : 
effectPtr(NULL), 
masterCallback(NULL),
blockSize(0),
sampleRate(0.f)

{

}
//-----------------------------------------------------------------------------
void VstForxPlug::open() {
	hostInfoAdapter = IHostInfo::Ptr(new HostInfoAdapter(*this));
	graph = ::processing::Graph::create(hostInfoAdapter);
	updateGraphBaseConfiguration();
}
//-----------------------------------------------------------------------------
void VstForxPlug::close() {
	graph.reset();
}
//-----------------------------------------------------------------------------
VstForxPlug::~VstForxPlug() {
}
//-----------------------------------------------------------------------------
void VstForxPlug::process(float **in, float **out, int numSamples) {
	if ( !graph ) 
		return;
	::processing::Frames fr ( in, numSamples ); 
	TRY_TO_LOCK_TIMED2 ( graph->getProcessingLock(), 30 );
	if ( !graph->getEndNode()->isActive() ){
		fr.setZero ( numSamples );
		fr.getBlock ( out, numSamples );
		return;
	} 
	graph->pushAndCopy ( &fr, numSamples );
	graph->processGraph( out, numSamples  );
}
//-----------------------------------------------------------------------------
void VstForxPlug::setBlockSize(int blockSize) {
	this->blockSize = blockSize; 
	updateGraphBaseConfiguration();
}
//-----------------------------------------------------------------------------
void VstForxPlug::setSampleRate(float sampleRate)  {
	this->sampleRate = sampleRate; 
	updateGraphBaseConfiguration();
}
//-----------------------------------------------------------------------------
void VstForxPlug::setParameterValue(int index, float value) {
}
//-----------------------------------------------------------------------------
void VstForxPlug::getParameterValue(int index, float &outValue) {
}
//-----------------------------------------------------------------------------
float VstForxPlug::getSampleRate() const  {
	return sampleRate;
}
//-----------------------------------------------------------------------------
int VstForxPlug::getBlockSize() const  {
	return blockSize;
}
//-----------------------------------------------------------------------------
bool VstForxPlug::ioChanged() {
	EventSender<HostIOChanged>::notifyListeners(this, HostIOChanged());
	return true;
}
//-----------------------------------------------------------------------------
TimeInfo * VstForxPlug::getHostTimeInfo (int filter) {
	return NULL;
}
//-----------------------------------------------------------------------------
VstForxPlug::HostIOChangedConnection 
VstForxPlug::addHostChangedListener(const HostIOChangedFunction &f) {
	return EventSender<HostIOChanged>::addEventListener(f);
}
//-----------------------------------------------------------------------------
VstForxPlug::HostIOChangedConnection 
VstForxPlug::addTrackedHostChangedListener(const HostIOChangedFunction &f, AnyWPtr wptr)
{
	return EventSender<HostIOChanged>::addTrackedEventListener(f, wptr);
}
//-----------------------------------------------------------------------------
void VstForxPlug::setEffectPtr(void *effPtr) {
	this->effectPtr = effPtr;
}
//-----------------------------------------------------------------------------
void VstForxPlug::setMasterCallback(void *mCallbk) {
	this->masterCallback = mCallbk;
}
//-----------------------------------------------------------------------------
void * VstForxPlug::getEffectPtr() {
	return effectPtr;
}
//-----------------------------------------------------------------------------
void * VstForxPlug::getMasterCallback() {
	return masterCallback;
}
//-----------------------------------------------------------------------------
void VstForxPlug::updateGraphBaseConfiguration() {
	if (!graph)
		return;
	if (sampleRate==0. || blockSize == 0)
		return;
	graph->getJanitor()->hostBaseConfigChanged();
}
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
IModelController *
getModelController(frx::gui::components::FrxCircuidViewPtr view)
{
	return NULL;
}
}} // namespace(s)