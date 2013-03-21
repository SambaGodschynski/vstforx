/*
 * ===========================================================================================================
 * ADSRTrigger.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_ADSRTRIGGER_H
#define FORX_ADSRTRIGGER_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/Serialization.h"
#include "processing/dspTools.h"

namespace processing {
using namespace parameter;
//============================================================================================================
/**
 * @class ADSRTRigger
 * Triggert Eingangs-Signal als Gate fuer ADSR-Verlauf
 */
class ADSRTrigger : public ProcessAdapter, 
	public HasParameter,
	public HasOutParameter,
	public Serializable {
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<ADSRTrigger> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert PeakTracker-Objekt
	 * @param ar boost::Archive-
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object < ProcessAdapter > ( *this );
		ar & out;
		ar & adsr;
	}
	//--------------------------------------------------------------------------------------------------------
	ADSRTrigger (){}
	//--------------------------------------------------------------------------------------------------------
	ADSR *adsr;
protected:
	//--------------------------------------------------------------------------------------------------------
	ADSRTrigger ( frx::processing::IHostInfo::Ptr hostInfo );
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @return neues PeakTracker-Objekt
	 */
	static Ptr create( frx::processing::IHostInfo::Ptr hostInfo ) {
		Ptr neu( new ADSRTrigger(hostInfo ) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	//Lautstaerke-wert
	Parameter::Ptr out;
	//--------------------------------------------------------------------------------------------------------
	virtual void hostBaseConfigChanged() { 
		adsr->hostBaseConfigChanged();
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge der Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return liefert Parameter zu index. Wirft: std::out_of_range
	 */
	virtual Parameter::Ptr getParameter ( size_t index = 0 ) const {
		if ( index > adsr->getNumParameter() ) return Parameter::Ptr();
		return adsr->getParameter(index);
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller ADSRTrigger-Parameter
	 */
	virtual size_t getNumParameter () const { return adsr->getNumParameter(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller ADSRTrigger-Parameter
	 */
	virtual size_t getNumOutParameter () const { return 1; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Ausgabe-Parameter
	 */
	Parameter::Ptr getOutParameter(size_t index = 0) const { return out; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~ADSRTrigger ();
};
}// namespace processing

#endif  // FORX_ADSRTRIGGER_H


