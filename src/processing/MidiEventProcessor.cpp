/*
 * ============================================================================
 * MidiEventProcessor.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */

#include "MidiEventProcessor.h"

namespace {
	enum { ALL_CHANNEL = 16 };
}

namespace processing{
//=============================================================================
// MidiEventProcessor
//=============================================================================
//-----------------------------------------------------------------------------
MidiEventProcessor::MidiEventProcessor() {
	using namespace parameter;
    midiChannel = parameter::Parameter::create();
	Parameter::ValueChangedFunction f = boost::bind(
		&MidiEventProcessor::midiChannelChanged, this, _1, _2 
	);
	midiChannel->addValueChangedListener ( f );
	midiChannel->setValue ( 1.0f );
	midiChannel->setName ("midi channel");
    
    midiSend = parameter::Parameter::create();
    f = boost::bind( 
		&MidiEventProcessor::midiSendChanged, this, _1, _2 
	);
	midiSend->addValueChangedListener ( f );
	midiSend->setValue ( 1.0f );
	midiSend->setName ("send midi");
}
//-----------------------------------------------------------------------------
void MidiEventProcessor::processEvents( sambag::dsp::IMidiEvents *ev ) {
	
	int channel = com::mapInteger ( midiChannel->getValue(), 17 ); // 16 midi channels + all channels

	if (channel!=ALL_CHANNEL) {
		tmpEv.copyDeepFiltered(ev, channel);
		processMidiEvents(&tmpEv);
		return;
	}
	processMidiEvents(ev);
}
//-----------------------------------------------------------------------------
void MidiEventProcessor::midiChannelChanged ( void *src, const float &val ) {
	using namespace parameter;
	int n = com::mapInteger ( midiChannel->getValue(), 17 ); // 16 midi channels + all channels
	midiChannel->setDisplay ( ( n==(size_t)ALL_CHANNEL ? "all" : com::MyString(n+1) ) ); 
}
//-----------------------------------------------------------------------------
void MidiEventProcessor::midiSendChanged ( void *src, const float &val ) {
	using namespace parameter;
	int n = com::mapInteger ( midiSend->getValue(), 2 );
	midiSend->setDisplay( (n==0 ? "no" : "yes") );
}
//-----------------------------------------------------------------------------
IMidiEventProcessor::Connection
MidiEventProcessor::addListener(const IMidiEventProcessor::EventFunction &f)
{
    return IMidiEventProcessor::EventSender::addEventListener(f);
}
//-----------------------------------------------------------------------------
void MidiEventProcessor::sendMidiEvents( sambag::dsp::IMidiEvents * events ) {
    using namespace parameter;
	if (com::mapInteger(midiSend->getValue(),2)==0) {
        return;
    }
    IMidiEventProcessor::EventSender::notifyListeners(this, events);
}
//-----------------------------------------------------------------------------
IMidiEventProcessor::Connection
MidiEventProcessor::addTrackedListener(const IMidiEventProcessor::EventFunction &f,
    AnyWPtr trackingObject)
{
    return IMidiEventProcessor::EventSender::addTrackedEventListener(f, trackingObject);
}
}//namespace processing
