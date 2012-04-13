/*
 * ===========================================================================================================
 * SyncTranslator.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_SYNCTRANSLATOR_H
#define FORX_SYNCTRANSLATOR_H


#include "com/Serialization.h"
#include "processing/dspTools.h"
#include "processing/IHostInfo.h"
#include "ValueTranslator.h"

namespace processing {
using namespace parameter;
//============================================================================================================
/**
 * @class SyncTranslator
 * Transformiert einen Eingabewert im bereich 0..1, in einen Musiknoten-Zeitwert in Samples,
 * abhaengig von Host-BPM und Samplerate.
 */
class SyncTranslator : public ValueTranslator {
//============================================================================================================
friend class boost::serialization::access;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert SyncTranslator.
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object < ValueTranslator > ( *this );
		ar & hostInfo;
	}
	//--------------------------------------------------------------------------------------------------------
	SyncTranslator() {}
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Anzahl der Samples die eine Millisekunde ergeben
	 */
	float oneMsInSamples;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * HostInfo-Objekt
	 */
	IHostInfo * hostInfo;
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * resetet SampleRate
	 */
	void hostInfoChanged() {
		oneMsInSamples = hostInfo->getSampleRate()/1000.0f; 
	}
	//--------------------------------------------------------------------------------------------------------
	SyncTranslator( IHostInfo * hostInfo ): 
	hostInfo(hostInfo), 
	oneMsInSamples ( hostInfo->getSampleRate()/1000.0f ) {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param v
	 * @return Eingabewert als einen Musiknoten-String zb.: 1/4, etc.
	 *
	 */
	virtual com::MyString  translateAsString ( float v ) {
		int n = mapInteger ( v, musicalValues::NUM_STDNOTES );
		return musicalValues::noteLengthTable[n].str;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param v
	 * @return Eingabewert als einen Musiknoten-Zeitwert in Samples
	 */
	virtual int translate ( float v ){
		int n = mapInteger ( v, musicalValues::NUM_STDNOTES );
		VstTimeInfo *inf = hostInfo->getVstTimeInfo( kVstTempoValid );
		return note2Sample ( musicalValues::noteLengthTable[n].val, inf->tempo, inf->sampleRate );
	}
};
}// namespace processing

#endif  // FORX_SYNCTRANSLATOR_H


