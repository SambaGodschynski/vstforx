/*
 * ===========================================================================================================
 * Volume.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "Volume.h"
#include <com/Serialization.h>
#include <boost/static_assert.hpp>

namespace processing {
//============================================================================================================
// class Volume
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void Volume::processAdapter( Processor::Int numSamples ) { 
	Frames *fr = getInputNode(0)->popFrame();
	int i = numSamples;
	com::VstNumber *r = (*fr)[0];
	com::VstNumber *l = (*fr)[1];
	while ( --i >= 0 ){
		float f = fader; // mit jedem lesezugriff wird fader::t erhoet!
		*(r++) *= f;
		*(l++) *= f;
	}
	outputNodes[0]->pushAndCopy( fr, numSamples );
}
}// namespace processing

#include <processing/ModelFactory.hpp>
namespace processing {
FRX_MODELFACTORY_REGISTER(internal, Volume);
} // namespace processing
