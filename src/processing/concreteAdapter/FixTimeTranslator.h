/*
 * ===========================================================================================================
 * FixTimeTranslator.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_FIXTIMETRANSLATOR_H
#define FORX_FIXTIMETRANSLATOR_H

#include "com/Serialization.h"
#include "ValueTranslator.h"

namespace processing {
//============================================================================================================
/**
 * @class FixTimeValue
 * Transformiert einen Eingabewert im bereich 0..1, in einen Millisekunden-Zeitwert in Samples.
 */
class FixTimeValue : public ValueTranslator {
//============================================================================================================
friend class boost::serialization::access;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert FixTimeValue-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object < ValueTranslator > ( *this );
		ar & oneMsInSamples;
		ar & minInSampl;
		ar & fak;
		ar & minInMs;
	}
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Anzahl der Samples die eine Millisekunde ergeben
	 */
	float oneMsInSamples;
	//--------------------------------------------------------------------------------------------------------
	float minInSampl;
	//--------------------------------------------------------------------------------------------------------
	float fak;
	//--------------------------------------------------------------------------------------------------------
	float minInMs;
	//--------------------------------------------------------------------------------------------------------
	FixTimeValue() {}
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param minInMs Zeitwertminimum in Millisekunden
	 * @param minInMs Samplerate in Hz
	 */
	FixTimeValue( float minInMs, float sampleRate ) :
	  minInMs(minInMs),
	  oneMsInSamples ( sampleRate/1000.0f ),
	  minInSampl ( oneMsInSamples * minInMs ),
	  fak(99.0f) {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param sampleRate neue SampleRate
	 * TODO: rename to setSampleRate
	 */
	void hostInfoChanged( float sampleRate ) {
		oneMsInSamples = sampleRate/1000.0f; 
		minInSampl = oneMsInSamples * minInMs;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param v
	 * @return v als Millisekunden String: z.B. 10ms
	 */
	virtual com::MyString  translateAsString ( float v ) {
		assert ( oneMsInSamples != 0.0f );
		float y = fak * v + 1.0f; // abbildung 0.0..1.0 => 1.0 100.0
		return com::MyString(y*minInSampl/oneMsInSamples) + " ms";
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param v
	 * @return Eingabewert im bereich 0..1, in einen Millisekunden-Zeitwert in Samples.
	 */
	virtual int translate ( float v ){
		return (int)( (fak * v + 1.0f) * minInSampl);
	}
};

}// namespace processing

#endif  // FORX_FIXTIMETRANSLATOR_H


