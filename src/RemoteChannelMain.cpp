#include "RemoteChannelMain.hpp"
#include "com/One4All.h"
#include <stdlib.h>
#include <sambag/dsp/VST2xPluginWrapper.hpp>
#include <sstream>

//=============================================================================
// class RemoteChannelProcessor 
//=============================================================================
//-----------------------------------------------------------------------------
int RemoteChannelProcessor::instances = 0;
//-----------------------------------------------------------------------------
RemoteChannelProcessor::RemoteChannelProcessor() : volume(0) {
	using namespace processing;
	std::stringstream ss;
	instance = instances++;
	ss << "remoteChannel " << instance;
	name = ss.str();
	remoteChannel = 
		RemoteChannelManager::instance()->createRemoteChannel(name);
	if (instance==0) {
		buffer =
			&RemoteChannelManager::instance()->getChannelBuffer(*remoteChannel);
		buffer->resize(255);
	}
	if (instance==1) {
		const RemoteChannel &remoteChannel = 
			RemoteChannelManager::instance()->getRegisteredChannels()[0];
		buffer =
			&RemoteChannelManager::instance()->getChannelBuffer(remoteChannel);
	}
}
//-----------------------------------------------------------------------------
RemoteChannelProcessor::~RemoteChannelProcessor() {
	using namespace processing;
	RemoteChannelManager::instance()->removeRemoteChannel(name);
}
//-----------------------------------------------------------------------------
void writeBuffer(float **in, float **out, processing::RemoteChannel::Buffer &bff) {
	using namespace processing;
	for (size_t i=0; i<255; ++i) {
		bff[i] = in[0][i];
	}
}
//-----------------------------------------------------------------------------
void readBuffer(float **in, float **out, processing::RemoteChannel::Buffer &bff) {
	for (size_t i=0; i<255; ++i) {
		out[0][i] = bff[i];
		out[1][i] = bff[i];
	}
	/*// noisy
	for (int i=0; i<255; ++i) {
		out[0][i] = (float)rand()/(float)RAND_MAX;
		out[1][i] = (float)rand()/(float)RAND_MAX;
	}*/
}
//-----------------------------------------------------------------------------
void RemoteChannelProcessor::process(float **in, float **out, int numSamples) {
	if (instance==0) {
		writeBuffer(in, out, *buffer);
	}
	if (instance==1) {
		readBuffer(in, out, *buffer);
	}
}

//-----------------------------------------------------------------------------
AudioEffect * createEffectInstance ( audioMasterCallback audioMaster ) {
	
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); //VS memory tracking

	using namespace sambag::dsp::vst;
	// settingup plugin
	typedef VST2xPluginWrapper<
		RemoteChannelProcessor, // Processor
		'frxr', // uid
		sambag::dsp::StdPluginTraits<1,1,false,1> // plugin constructor
	> Plugin;
	// create plugin
	try{
		return new Plugin(audioMaster);
	}catch(...){
		MessageBox ( NULL, "Could not create RemoteChannel "
			"Effect Instance!", "Error!", 0 );
		return NULL;
	}
}