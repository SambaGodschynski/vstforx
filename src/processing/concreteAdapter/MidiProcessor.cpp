                                        /*
 * ===========================================================================================================
 * MidiProcessor.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include <tuple>
#include <boost/tuple/tuple.hpp>
#include "MidiProcessor.h"
#include <com/Serialization.h>
#include "processing/dspTools.h"
#include <sambag/dsp/IMidiEvents.hpp>

namespace processing{
//============================================================================================================
// MidiProcessor:
// One MidiProcessor per channel.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
MidiProcessor::MidiProcessor ( frx::processing::IHostInfo::Ptr iHost ) :
		ProcessAdapter( iHost, 0, 0 ),
		midiParameters( NUM_OUT_PARAM, parameter::Parameter::Ptr() )
{
	setName ("midi_receiver");
	initParameter ( PITCH_BEND , "pitchbend" );
	midiParameters[PITCH_BEND]->setValue(0.5f);
	for ( size_t i=(size_t)NUM_NO_CC_PARAMETER; i<(size_t)NUM_OUT_PARAM; ++i ) {
		size_t c = i - (size_t)NUM_NO_CC_PARAMETER;
		initParameter ( i, "cc(" + com::MyString(c) + ")" + musicalValues::getCCName(c) );
	}
}
//------------------------------------------------------------------------------------------------------------
void MidiProcessor::processMidiEvents ( sambag::dsp::IMidiEvents::Ptr ev ) {
	using namespace sambag::dsp;
	size_t numEv = ev->getNumEvents();
	for ( size_t i=0; i<numEv; ++i ) {
		IMidiEvents::ByteSize size;
		IMidiEvents::DeltaFrames d;
		IMidiEvents::DataPtr data;
		auto _mev = ev->getMidiEvent(i);
		size = boost::get<0>(_mev);
		d = boost::get<1>(_mev);
		data = boost::get<2>(_mev);

		for (int j=0; j<size-2;) { // through bytes
			Byte status = ( data[j] & 0xf0 ) >> 4;
			size_t gv=0, cc=0;
			switch ( status ) {
				case 0xE : // pitchbend
					gv = (data[j+2] << 7 ) | data[j+1];
					midiParameters[PITCH_BEND]->setValue( gv / 16383.0f );
					j+=2; // skip next 2 bytes
					continue;
				case 0xB : // cc
					cc = data[j+1];
					if ( cc >= (size_t)NUM_CC ) {
						j+=2; // skip next 2 bytes
						continue;
					}
					gv = data[j+2];
					midiParameters[ cc + (size_t)NUM_NO_CC_PARAMETER  ]->setValue( gv/127.0f );
					j+=2; // skip next 2 bytes
					continue;
			}
			++j; //next byte
		}
	}
}
}// namespace processing

#include <processing/ModelFactory.hpp>
namespace processing {
FRX_MODELFACTORY_REGISTER(internal, MidiProcessor);
} // namespace processing
