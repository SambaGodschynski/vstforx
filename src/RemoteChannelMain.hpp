/*
 * RemoteChannelMain.hpp
 *
 *  Created on: 02 April 2012
 *      Author: samba
 */
#ifndef REMOTECHANNEL_MAIN_HPP_
#define REMOTECHANNEL_MAIN_HPP_

#include <sambag/dsp/VST2xPluginWrapper.hpp>
#include <string>

//=============================================================================
struct ParameterAcessor {
	template <class Plugin, typename T>
	void setParameterValue(Plugin &plugin, int index, const T &value){
		plugin.volume = value;
	}
	template <class Plugin, typename T>
	void getParameterValue(const Plugin &plugin, int index, T &value){
		value = plugin.volume;
	}
	template <class Plugin, class String>
	void getParameterName (const Plugin &plugin, int index, String &outStr){
		outStr = "volume";
	}
	template <class Plugin, class String>
	void getParameterLabel(const Plugin &plugin, int index, String &outStr){
	}
	template <class Plugin, class String>
	void getParameterDisplay(const Plugin &plugin, int index, String &outStr){
	}
};

//=============================================================================
class RemoteChannelProcessor {
//=============================================================================
friend struct ParameterAcessor;
private:
	//-------------------------------------------------------------------------
	float volume;
public:
	//-------------------------------------------------------------------------
	RemoteChannelProcessor() : volume(0) {}
	//-------------------------------------------------------------------------
	void process(float **in, float**out, VstInt32 numSamples);
};

#endif //VSTFORX_MAIN_HPP_