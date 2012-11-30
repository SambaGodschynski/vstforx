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
	/*typedef unsigned char Byte;
	Byte *rawData = NULL;
	sambag::dsp::IMidiEvents *tmpEvents = NULL;
	if ( ev->numEvents <= (size_t)EVENTS_OVERHEAD ) { // overhead sufficient
		tmpEvents = (VstEvents*) &staticEvent; // use static event
	}
	else { // numEvents > overhead => create dynamic event 
		size_t size = sizeof(VstEvents) + (ev->numEvents) * sizeof(VstEvent*);
		rawData = new Byte[size];
		tmpEvents = (VstEvents*)rawData;
	}
	filterEvents ( ev, tmpEvents ); 
	processMidiEvents ( tmpEvents );
	// free rawData if used
	if (rawData) 
		delete rawData;*/
}
//------------------------------------------------------------------------------------------------------------
inline void MidiEventProcessor::filterEvents( sambag::dsp::IMidiEvents * src, sambag::dsp::IMidiEvents * dst ) {
/*	using namespace parameter;
	int n = mapInteger ( midiChannel->getValue(), 17 ); // 16 midi channels + all channels

	dst->numEvents = 0;

	// copy all events assigned to midi channel to eventBuffer
	for ( int i=0; i<src->numEvents; ++i ) {
		if ( (src->events[i])->type != kVstMidiType ) continue; // is no midi ev
		VstMidiEvent* _event = (VstMidiEvent*)src->events[i];
		// get channel
		char* midiData = _event->midiData;
		VstInt32 channel = midiData[0] & 0xf;	
		if ( channel != n && n!=(size_t)ALL_CHANNEL ) continue;
		dst->events[dst->numEvents++] = src->events[i];
	}*/
}
//------------------------------------------------------------------------------------------------------------
void MidiEventProcessor::midiChannelChanged ( void *src, const float &val ) {
	using namespace parameter;
	int n = mapInteger ( midiChannel->getValue(), 17 ); // 16 midi channels + all channels
	midiChannel->setDisplay ( ( n==(size_t)ALL_CHANNEL ? "all" : MyString(n+1) ) ); 
}
}//namespace processing
