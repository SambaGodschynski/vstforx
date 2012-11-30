                                        /*
 * ===========================================================================================================
 * Volume.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "Volume.h"


namespace processing{
//============================================================================================================
// class Volume
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void Volume::processAdapter( Processor::Int numSamples ) { 
	Frames *fr = getInputNode(0)->popFrame();
	int i = numSamples;
	VstNumber *r = (*fr)[0];
	VstNumber *l = (*fr)[1];
	while ( --i >= 0 ){
		float f = fader; // mit jedem lesezugriff wird fader::t erhoet!
		*(r++) *= f;
		*(l++) *= f;
	}
	outputNodes[0]->pushAndCopy( fr, numSamples );
}
}// namespace processing
