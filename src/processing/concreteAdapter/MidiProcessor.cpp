                                        /*
 * ===========================================================================================================
 * MidiProcessor.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "MidiProcessor.h"
#include "processing/dspTools.h"

namespace processing{
//============================================================================================================
// MidiProcessor:
// One MidiProcessor per channel.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
MidiProcessor::MidiProcessor ( IHostInfo *iHost ) :
		ProcessAdapter( iHost, 0, 0 ),
		midiParameters( NUM_OUT_PARAM, Parameter::Ptr() )
{
	setName ("midi_receiver");
	initParameter ( PITCH_BEND , "pitchbend" );
	midiParameters[PITCH_BEND]->setValue(0.5f);
	for ( size_t i=(size_t)NUM_NO_CC_PARAMETER; i<(size_t)NUM_OUT_PARAM; ++i ) {
		size_t c = i - (size_t)NUM_NO_CC_PARAMETER;
		initParameter ( i, "cc(" + MyString(c) + ")" + musicalValues::getCCName(c) );
	}
	TOLOG ( "+" + getName() );
}
//------------------------------------------------------------------------------------------------------------
void MidiProcessor::processMidiEvents ( VstEvents *ev ) {
	for ( size_t i=0; i<ev->numEvents; ++i ) {
		VstMidiEvent *midiEv = (VstMidiEvent*)ev->events[i];
		Byte status = ( midiEv->midiData[0] & 0xf0 ) >> 4;
		size_t gv=0, cc=0;
		switch ( status ) {
			case 0xE /*pitchbend*/ :
				gv = ( midiEv->midiData[2] << 7 ) | midiEv->midiData[1];
				midiParameters[PITCH_BEND]->setValue( gv / 16383.0f );
				break;
			case 0xB /*cc*/        :
				cc = midiEv->midiData[1];
				if ( cc >= (size_t)NUM_CC ) continue;
				gv = midiEv->midiData[2];
				midiParameters[ cc + (size_t)NUM_NO_CC_PARAMETER  ]->setValue( gv/127.0f );
				break;
		}
	}
}
}// namespace processing
