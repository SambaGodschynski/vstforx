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
	ss << "remoteChannel " << instances++;
	name = ss.str();
	remoteChannel = 
		RemoteChannelManager::instance()->createRemoteChannel(name);
}
//-----------------------------------------------------------------------------
RemoteChannelProcessor::~RemoteChannelProcessor() {
	using namespace processing;
	RemoteChannelManager::instance()->removeRemoteChannel(name);
}
//-----------------------------------------------------------------------------
void RemoteChannelProcessor::process(float **in, float **out, int numSamples) {
	// noisy
	for (int i=0; i<numSamples; ++i) {
		out[0][i] = (float)rand()/(float)RAND_MAX * volume;
		out[1][i] = (float)rand()/(float)RAND_MAX * volume;
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
