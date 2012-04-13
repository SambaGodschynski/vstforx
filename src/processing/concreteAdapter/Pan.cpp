                                        /*
 * ===========================================================================================================
 * Pan.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "Pan.h"


namespace processing{
//============================================================================================================
// class Pan
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
Pan::Pan ( IHostInfo *hostInfo ) : ProcessAdapter( hostInfo, 1, 1 ) {
	setName ("Pan");
	getOutputNode(0)->setName ("PanAdapter_Output");
	getInputNode(0)->setName ("PanAdapter_Input");
	TOLOG ( "+" + getName() );
	
	pan = Parameter::create();
	pan->setName ("Pan");
	pan->setValue (0.5);
}
//------------------------------------------------------------------------------------------------------------
void Pan::processAdapter( Processor::Int numSamples ) {
	Frames *fr = getInputNode(0)->popFrame();
	float *l = (*fr)[0];
	float *r = (*fr)[1];
	Frames::Int c = numSamples;
	while ( c-- > 0 ) {
		*l = *l * ( 1.0f - *pan );
		*r = *r * *pan;
		l++; r++;
	}
	getOutputNode(0)->pushAndCopy ( fr, numSamples );
}
}// namespace processing
