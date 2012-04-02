#include "RemoteChannelMain.hpp"
#include "com/One4All.h"
#include <stdlib.h>
//=============================================================================
// class RemoteChannelProcessor 
//=============================================================================
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
		StdPluginTraits<1,1,false,1>, // plugin constructor
		ParameterAcessor
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
