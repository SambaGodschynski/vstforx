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
	
	int channel = mapInteger ( midiChannel->getValue(), 17 ); // 16 midi channels + all channels

	if (channel!=ALL_CHANNEL) {
		sambag::dsp::DefaultMidiEvents filtered;
		filterEvents (ev, &filtered, channel); 	
		processMidiEvents(&filtered);
		return;
	}
	processMidiEvents(ev);
}
//------------------------------------------------------------------------------------------------------------
inline void MidiEventProcessor::filterEvents( sambag::dsp::IMidiEvents * src, 
	sambag::dsp::DefaultMidiEvents * dst, int channel) 
{
	using namespace parameter;
	typedef sambag::dsp::IMidiEvents::MidiEvent MidiEvent;
	typedef sambag::dsp::IMidiEvents::DataPtr DataPtr;
	typedef sambag::dsp::IMidiEvents::Int MidiInt;

	MidiInt num = src->getNumEvents();

	// copy all events assigned to midi channel to eventBuffer
	dst->reserve(num);
	for (MidiInt i=0; i<num; ++i) {
		MidiEvent _event = src->getMidiEvent(i);
		// get channel
		DataPtr midiData = boost::get<2>(_event);
		int n = midiData[0] & 0xf;	
		if (n != channel) 
			continue;
		dst->insertFlat(_event);
	}
}
//------------------------------------------------------------------------------------------------------------
void MidiEventProcessor::midiChannelChanged ( void *src, const float &val ) {
	using namespace parameter;
	int n = mapInteger ( midiChannel->getValue(), 17 ); // 16 midi channels + all channels
	midiChannel->setDisplay ( ( n==(size_t)ALL_CHANNEL ? "all" : MyString(n+1) ) ); 
}
}//namespace processing
