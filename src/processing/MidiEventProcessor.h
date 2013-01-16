/*
 * ===========================================================================================================
 * MidiEventProcessor.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef MIDI_EVENT_PROCESSOR_H_
#define MIDI_EVENT_PROCESSOR_H_

#include "processing/parameter/Parameter.h"
#include "boost/shared_ptr.hpp"
#include "IMidiEventProcessor.h"
#include "processing/processing.h"
#include <sambag/dsp/DefaultMidiEvents.hpp>
#include <vector>

namespace processing {
//============================================================================================================
/** 
 * @class MidiEventProcessor.
 *  verarbeitet midi events. channel filterbar.
 *  TODO: seperate midiEvent / vstMidiEvent
 */
class MidiEventProcessor : public IMidiEventProcessor {
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<MidiEventProcessor> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	parameter::Parameter::Ptr midiChannel;
	//--------------------------------------------------------------------------------------------------------
	template <typename Archive>
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & midiChannel;
		// -=-=-=-=-=-=-=-=-=-=-=- 
		if ( Archive::is_loading::value == true ) {
			parameter::Parameter::ValueChangedFunction f = boost::bind( 
				&MidiEventProcessor::midiChannelChanged, this, _1, _2 
			);
			midiChannel->addValueChangedListener ( f );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void midiChannelChanged ( void *src, const float &val );
	//--------------------------------------------------------------------------------------------------------
	sambag::dsp::DefaultMidiEvents tmpEv;
public:
	//--------------------------------------------------------------------------------------------------------
	MidiEventProcessor();
	//--------------------------------------------------------------------------------------------------------
	virtual void processEvents( sambag::dsp::IMidiEvents * events );
	//--------------------------------------------------------------------------------------------------------
	virtual void processMidiEvents( sambag::dsp::IMidiEvents * events ) = 0;
	//--------------------------------------------------------------------------------------------------------
	parameter::Parameter::Ptr getMidiChannelParameter() { return midiChannel; }
};
} // processing
#endif