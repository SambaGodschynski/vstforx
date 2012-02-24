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
#include "IVstEventProcessor.h"
#include "processing/processing.h"
#include <vector>

namespace processing {
//============================================================================================================
// MidiEventProcessor :
// verarbeitet midi events. channel filterbar
//============================================================================================================
class MidiEventProcessor : public IVstEventProcessor {
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<MidiEventProcessor> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	enum { EVENTS_OVERHEAD = 1024 };
	//--------------------------------------------------------------------------------------------------------
	struct {
		VstInt32 numEvents;		
		VstIntPtr reserved;		
		VstEvent *events[EVENTS_OVERHEAD];
	} staticEvent;
	//--------------------------------------------------------------------------------------------------------
	parameter::Parameter::Ptr midiChannel;
	//--------------------------------------------------------------------------------------------------------
	template <typename Archive>
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & boost::serialization::base_object<ProcessAdapter> ( *this );
		ar & midiChannel;
		// -=-=-=-=-=-=-=-=-=-=-=- 
		if ( Archive::is_loading::value == true ) {
			parameter::Parameter::ValueChangedFunction f = boost::bind( 
				&MidiEventProcessor::midiChannelChanged, this, _1, _2 
			);
			midiChannel->addValueChangedListenerF ( f );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void midiChannelChanged ( void *src, const float &val );
public:
	//--------------------------------------------------------------------------------------------------------
	MidiEventProcessor();
	//--------------------------------------------------------------------------------------------------------
	// copys vstevents considers midi channel.
	inline void filterEvents( VstEvents * scr, VstEvents * dst );
	//--------------------------------------------------------------------------------------------------------
	virtual void processEvents( VstEvents * events );
	//--------------------------------------------------------------------------------------------------------
	virtual void processMidiEvents( VstEvents * events ) = 0;
	//--------------------------------------------------------------------------------------------------------
	parameter::Parameter::Ptr getMidiChannelParameter() { return midiChannel; }
};
} // processing
#endif