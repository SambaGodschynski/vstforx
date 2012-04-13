/*
 * ===========================================================================================================
 * PeakTracker.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_PEAKTRACKER_H
#define FORX_PEAKTRACKER_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/Serialization.h"


namespace processing {
using namespace parameter;
//============================================================================================================
/**
 * @class PeakTracker
 * Tranformiert Signal in Parameter wert.
 */
class PeakTracker : public ProcessAdapter, public HasParameter, public Serializable {
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<PeakTracker> Ptr;
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
		ar & offset;
		ar & out;
	}
	//--------------------------------------------------------------------------------------------------------
	PeakTracker (){}
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr offset;
protected:
	//--------------------------------------------------------------------------------------------------------
	PeakTracker ( IHostInfo *hostInfo );
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @return neues PeakTracker-Objekt
	 */
	static Ptr create( IHostInfo *hostInfo ) {
		Ptr neu( new PeakTracker(hostInfo ) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Ausgabe-Parameter
	 */
	Parameter::Ptr out;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge der Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return liefert OffsetParameter
	 */
	virtual Parameter::Ptr getParameter ( size_t index = 0 ) const { return offset; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return  Ausgabe-Parameter
	 */
	Parameter::Ptr getOutParameter() { return out; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller PeakTracker-Parameter
	 */
	virtual size_t getNumParameter () const { return 1; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~PeakTracker (){ 
		TOLOG ( "-" + getName() );
	}
};
}// namespace processing

#endif  // FORX_PEAKTRACKER_H


