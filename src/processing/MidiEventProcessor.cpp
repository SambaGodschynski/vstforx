/*
 * ===========================================================================================================
 * MidiEventProcessor.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "MidiEventProcessor.h"

namespace {
	enum { ALL_CHANNEL = 16 };
}

namespace processing{
//============================================================================================================
// MidiEventProcessor
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
MidiEventProcessor::MidiEventProcessor() : midiChannel( parameter::Parameter::create() ) {
	using namespace parameter;
	Parameter::ValueChangedFunction f = boost::bind( 
		&MidiEventProcessor::midiChannelChanged, this, _1, _2 
	);
	midiChannel->addValueChangedListener ( f );
	midiChannel->setValue ( 1.0f );
	midiChannel->setName ("midi channel"); 
}
//------------------------------------------------------------------------------------------------------------
void MidiEventProcessor::processEvents( sambag::dsp::IMidiEvents *ev ) {
	
	int channel = com::mapInteger ( midiChannel->getValue(), 17 ); // 16 midi channels + all channels

	if (channel!=ALL_CHANNEL) {
		tmpEv.copyDeepFiltered(ev, channel);
		processMidiEvents(&tmpEv);
		return;
	}
	processMidiEvents(ev);
}
//------------------------------------------------------------------------------------------------------------
void MidiEventProcessor::midiChannelChanged ( void *src, const float &val ) {
	using namespace parameter;
	int n = com::mapInteger ( midiChannel->getValue(), 17 ); // 16 midi channels + all channels
	midiChannel->setDisplay ( ( n==(size_t)ALL_CHANNEL ? "all" : com::MyString(n+1) ) ); 
}
}//namespace processing
