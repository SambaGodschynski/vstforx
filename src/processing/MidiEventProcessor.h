/*
 * ============================================================================
 * MidiEventProcessor.h
 *      Author: Johannes Unger
 * ============================================================================
 */

#ifndef MIDI_EVENT_PROCESSOR_H_
#define MIDI_EVENT_PROCESSOR_H_

#include "processing/parameter/parameter.h"
#include "boost/shared_ptr.hpp"
#include "IMidiEventProcessor.h"
#include "processing/processing.h"
#include <sambag/dsp/DefaultMidiEvents.hpp>
#include <vector>

namespace processing {
//=============================================================================
/** 
 * @class MidiEventProcessor.
 */
class MidiEventProcessor :
    public IMidiEventProcessor,
    public IMidiEventProcessor::EventSender
{
//=============================================================================
friend class boost::serialization::access;
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<MidiEventProcessor> Ptr;
private:
	//-------------------------------------------------------------------------
	parameter::Parameter::Ptr midiChannel, midiSend;
	//-------------------------------------------------------------------------
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
        if (version<=0) {
            return;
        }
        ar & midiSend;
        if ( Archive::is_loading::value == true ) {
			parameter::Parameter::ValueChangedFunction f = boost::bind( 
				&MidiEventProcessor::midiSendChanged, this, _1, _2 
			);
			midiSend->addValueChangedListener ( f );
		}
	}
	//-------------------------------------------------------------------------
	virtual void midiChannelChanged ( void *src, const float &val );
	//-------------------------------------------------------------------------
	virtual void midiSendChanged ( void *src, const float &val );
	//-------------------------------------------------------------------------
	sambag::dsp::DefaultMidiEvents::Ptr tmpEv;
public:
	//-------------------------------------------------------------------------
	MidiEventProcessor();
	//-------------------------------------------------------------------------
	virtual void processEvents( sambag::dsp::IMidiEvents::Ptr events );
    //-------------------------------------------------------------------------
	virtual void sendMidiEvents( sambag::dsp::IMidiEvents::Ptr events );
	//-------------------------------------------------------------------------
	virtual void processMidiEvents( sambag::dsp::IMidiEvents::Ptr events ) = 0;
	//-------------------------------------------------------------------------
	parameter::Parameter::Ptr getMidiChannelParameter() { return midiChannel; }
	//-------------------------------------------------------------------------
	parameter::Parameter::Ptr getMidiSendParameter() { return midiSend; }
    //-------------------------------------------------------------------------
    virtual IMidiEventProcessor::Connection
    addListener(const IMidiEventProcessor::EventFunction &f);
    //-------------------------------------------------------------------------
    virtual IMidiEventProcessor::Connection
    addTrackedListener(const IMidiEventProcessor::EventFunction &f,
        AnyWPtr trackingObject);

};
} // processing
BOOST_CLASS_VERSION(processing::MidiEventProcessor, 1);

#endif