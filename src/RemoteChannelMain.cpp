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
RemoteChannelProcessor::RemoteChannelProcessor() :
volume(0)
{
	using namespace processing;
	std::stringstream ss;
	instance = instances++;
	ss << "RemoteChannelHandler " << instance;
	name = ss.str();
	RemoteChannelHandler = 
		getRemoteChannelManager()->createRemoteChannel(name);
	
	buffer = &(getRemoteChannelManager()->getChannelBuffer(RemoteChannelHandler));
	buffer->data.resize(255);
}
//-----------------------------------------------------------------------------
RemoteChannelProcessor::~RemoteChannelProcessor() {
	using namespace processing;
	getRemoteChannelManager()->releaseChannelBuffer(RemoteChannelHandler);
	getRemoteChannelManager()->releaseChannel(RemoteChannelHandler);
}
//-----------------------------------------------------------------------------
void RemoteChannelProcessor::process(float **in, float **out, int numSamples) {
	buffer->write(in, numSamples);
	for (size_t i=0; i<numSamples; ++i) {
		out[0][i] = in[0][i] * volume;
		out[1][i] = in[1][i] * volume;
	}
}
//-----------------------------------------------------------------------------
AudioEffect * createEffectInstance ( audioMasterCallback audioMaster ) {
	
#if _WIN32
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); //VS memory tracking
#endif

	using namespace sambag::dsp::vst;
	// settingup plugin
	typedef VST2xPluginWrapper<
		RemoteChannelProcessor, // Processor
		'frxr', // uid
		sambag::dsp::StdPluginTraits<2,2,false,1> // plugin constructor
	> Plugin;
	// create plugin
	try{
		return new Plugin(audioMaster);
	}catch(...){
		com::MessageBox ( "Effect Instance!", "Could not create RemoteChannelHandler", com::MSG_ALERT);
		return NULL;
	}
}
