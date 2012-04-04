/*
 * RemoteChannelMain.hpp
 *
 *  Created on: 02 April 2012
 *      Author: samba
 */
#ifndef REMOTECHANNEL_MAIN_HPP_
#define REMOTECHANNEL_MAIN_HPP_

#include <sambag/dsp/DspPlugin.hpp>
#include <string>
#include "processing/RemoteChannel.h"

//=============================================================================
class RemoteChannelProcessor : public sambag::dsp::PluginProcessorBase {
//=============================================================================
private:
	//-------------------------------------------------------------------------
	int instance;
	//-------------------------------------------------------------------------
	processing::RemoteChannel remoteChannel, readChannel;
	//-------------------------------------------------------------------------
	processing::RemoteChannel::Buffer * buffer;
	//-------------------------------------------------------------------------
	float volume;
	//-------------------------------------------------------------------------
	static int instances;
	//-------------------------------------------------------------------------
	std::string name;
public:
	//-------------------------------------------------------------------------
	~RemoteChannelProcessor();
	//-------------------------------------------------------------------------
	RemoteChannelProcessor();
	//-------------------------------------------------------------------------
	void process(float **in, float**out, int numSamples);
	//-------------------------------------------------------------------------
	void setParameterValue(int index, float value) {
		volume = value;
	}
	//-------------------------------------------------------------------------
	void getParameterValue(int index, float &outValue) {
		outValue = volume;
	}
};

#endif //VSTFORX_MAIN_HPP_