/*
 * ============================================================================
 * FrqDetector.h
 *      Author: Johannes Unger
 * ============================================================================
 */
#ifndef FORX_FrqDetector_H
#define FORX_FrqDetector_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/Serialization.h"
#include "processing/dspTools.h"
#include <processing/ModelFactory.hpp>

namespace processing {
using namespace parameter;
//=============================================================================
/**
 * @class FrqDetector
 */
class FrqDetector : public ProcessAdapter, 
	public HasParameter,
	public HasOutParameter,
	public com::Serializable
{
//=============================================================================
friend class boost::serialization::access;
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrqDetector> Ptr;
private:
	//-------------------------------------------------------------------------
	/**
	 * (De)Serialisiert PeakTracker-Objekt
	 * @param ar boost::Archive-
	 * @param version
	 */
	void serialize ( ::com::iArchive &ar, const unsigned int version );
	void serialize ( ::com::oArchive &ar, const unsigned int version );
	//-------------------------------------------------------------------------
	FrqDetector (){}
    //-------------------------------------------------------------------------
    unsigned int sampleCounter;
    //-------------------------------------------------------------------------
    static const int WindowSize;
protected:
	//-------------------------------------------------------------------------
	FrqDetector ( frx::processing::IHostInfo::Ptr hostInfo );
public:
	//-------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @return neues PeakTracker-Objekt
	 */
	static Ptr create( frx::processing::IHostInfo::Ptr hostInfo ) {
		Ptr neu( new FrqDetector(hostInfo ) );
		neu->self = neu;
		return neu;
	}
	//-------------------------------------------------------------------------
	Parameter::Ptr frq;
	//-------------------------------------------------------------------------
	virtual void hostBaseConfigChanged();
	//-------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge der Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int numSamples );
	//-------------------------------------------------------------------------
	/**
	 * @param index
	 * @return liefert Parameter zu index. Wirft: std::out_of_range
	 */
	virtual Parameter::Ptr getParameter ( size_t index = 0 ) const {
		return Parameter::Ptr();
	}
	//-------------------------------------------------------------------------
	/**
	 * @return Anzahl aller FrqDetector-Parameter
	 */
	virtual size_t getNumParameter () const { 0; }
	//-------------------------------------------------------------------------
	/**
	 * @return Anzahl aller FrqDetector-Parameter
	 */
	virtual size_t getNumOutParameter () const { return 1; }
	//-------------------------------------------------------------------------
	/**
	 * @return Ausgabe-Parameter
	 */
	Parameter::Ptr getOutParameter(size_t index = 0) const { return frq; }
	//-------------------------------------------------------------------------
	virtual ~FrqDetector ();
};

FRX_MODELFACTORY_REGISTER(internal, FrqDetector);

}// namespace processing

#endif  // FORX_FrqDetector_H


