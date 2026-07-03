/*
 * ===========================================================================================================
 * MidiProcessor.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_MIDIPROCESSOR_H
#define FORX_MIDIPROCESSOR_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/SerializationFwd.h"
#include "processing/MidiEventProcessor.h"

namespace processing {
//============================================================================================================
/**
 * @class MidiProcessor.
 * Bildet MIDI-Events auf N Augabe-Parmeter ab.
 */
class MidiProcessor : 
public ProcessAdapter, 
public parameter::HasParameter, 
public com::Serializable,
public MidiEventProcessor
{
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<MidiProcessor> Ptr;
	//--------------------------------------------------------------------------------------------------------
	typedef unsigned char Byte;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * MIDI-Kanal-Auswahl  Parameter
	 */
	parameter::Parameter::Ptr channelSelector;
	//--------------------------------------------------------------------------------------------------------
	typedef std::vector<parameter::Parameter::Ptr> Parameters;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Parameter index, der Parameter die keinem MIDI-Conrol Event zugehoerig sind
	 */
	enum NoCCParameterIndex { PITCH_BEND, NUM_NO_CC_PARAMETER };
	//--------------------------------------------------------------------------------------------------------
	enum { NUM_CC = 128, NUM_OUT_PARAM = NUM_CC + NUM_NO_CC_PARAMETER };
	//--------------------------------------------------------------------------------------------------------
	Parameters midiParameters;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert PeakTracker-Objekt
	 * @param ar boost::Archive-
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & boost::serialization::base_object < ProcessAdapter > ( *this );
		ar & boost::serialization::base_object < MidiEventProcessor > ( *this );
		ar & midiParameters; 
		ar & channelSelector;
	}
	//--------------------------------------------------------------------------------------------------------
	void initParameter( size_t index, const std::string &name ) {
		midiParameters[index] = parameter::Parameter::create( index );
		midiParameters[index]->setName ( name );
	}
	//--------------------------------------------------------------------------------------------------------
	MidiProcessor (){} // wird nur von boost::serial. benutzt
protected:
	//--------------------------------------------------------------------------------------------------------
	MidiProcessor ( frx::processing::IHostInfo::Ptr hostInfo );
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet MidiEvent
	 * @param events
	 */
	virtual void processMidiEvents( sambag::dsp::IMidiEvents::Ptr events );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @return neues MidiProcessor-Objekt
	 */
	static Ptr create( frx::processing::IHostInfo::Ptr hostInfo ) {
		Ptr neu( new MidiProcessor(hostInfo ) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * TODO: wird wohl nicht benoetigt
	 * @param src
	 * @param value
	 */
	virtual void valueChanged ( void *src, const float &value ) {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return MIDI-Kanal-Auswahl-Parameter
	 */
	virtual parameter::Parameter::Ptr getMidiChannelParameter() { return channelSelector; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge der Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int numSamples ) {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return liefert Parameter zu index. Wirft: std::out_of_range
	 */
	virtual parameter::Parameter::Ptr getParameter ( size_t index = 0 ) const { return midiParameters[index]; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller MidiProcessor-Parameter
	 */
	virtual size_t getNumParameter () const { return midiParameters.size(); }
	//--------------------------------------------------------------------------------------------------------
	virtual ~MidiProcessor (){
	}
};


}// namespace processing

#endif  // FORX_MIDIPROCESSOR_H


