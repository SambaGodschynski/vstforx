/*
 * VstForxPlug.hpp
 *
 *  Created on: Mon Oct  8 12:58:07 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_VSTFORXPLUG_H
#define SAMBAG_VSTFORXPLUG_H

#include <boost/shared_ptr.hpp>
#include <sambag/dsp/DspPlugin.hpp>
#include <com/FrxConfig.h>
#include <processing/interprocess/RemoteChannelManager.hpp>
#include <processing/interprocess/Stream.hpp>

namespace frx { namespace processing { namespace remoteChannel {
//=============================================================================
class Plugin : public sambag::dsp::PluginProcessorBase {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef sambag::dsp::PluginProcessorBase Super;
private:
	//-------------------------------------------------------------------------
	int blockSize;
	//-------------------------------------------------------------------------
	float sampleRate;
    //-------------------------------------------------------------------------
    interprocess::Stream::Ptr stream;
    //-------------------------------------------------------------------------
    interprocess::RemoteChannelManager::RCId channelId;
protected:
	//-------------------------------------------------------------------------
	/**
	 * updates graph samplerate and blocksize
	 */
	void updateConfiguration();
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
public:
	//-------------------------------------------------------------------------
	void open();
	//-------------------------------------------------------------------------
	void close();
	//-------------------------------------------------------------------------
	void processEvents(sambag::dsp::IMidiEvents *ev);
	//-------------------------------------------------------------------------
	void hostParameterChanged(void *src, float value, int index);
	//-------------------------------------------------------------------------
	Plugin();
	//-------------------------------------------------------------------------
	~Plugin();
	//-------------------------------------------------------------------------
	void process(float **in, float**out, int numSamples);
	//-------------------------------------------------------------------------
	int getChunk(void **data);
	//-------------------------------------------------------------------------
	int setChunk(void *data, int byteSize);
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
		outStr = "VSTForx.RemoteChannel";
	} 
	//-------------------------------------------------------------------------
	int getProductVersion() const {
		return FRX_VERSION_MAJOR*1000 + FRX_VERSION_MINOR*100 + FRX_VERSION_MICRO;
	}
};
}}} // namespace

#endif /* SAMBAG_VSTFORXPLUG_H */
