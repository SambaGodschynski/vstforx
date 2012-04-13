/*
 * ===========================================================================================================
 * Pan.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_PAN_H
#define FORX_PAN_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/Serialization.h"


namespace processing {
using namespace parameter;
//============================================================================================================
/**
 * @class Pan.
 *  Pan auf Eingangs-Samplemenge.
 */
class Pan : public ProcessAdapter, public Serializable, public HasParameter  {
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<Pan> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert Pan-Objekt
	 * @param ar boost::Archive Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< ProcessAdapter > ( *this );
		ar & pan;
	}
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr pan;
	//--------------------------------------------------------------------------------------------------------
	Pan() {}
protected:
	//--------------------------------------------------------------------------------------------------------
	Pan ( IHostInfo *hostInfo );
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @return neues Pan-Objekt
	 */
	static Ptr create( IHostInfo *hostInfo ) {
		Ptr neu( new Pan(hostInfo) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge des Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return  ausschliesslich Panparameter, da einzger Parameter.
	 */
	virtual Parameter::Ptr getParameter ( size_t index = 0 ) const { return pan; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return 1
	 */
	virtual size_t getNumParameter () const { return 1; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~Pan() {
		TOLOG ( "-" + getName() );
	}

};
}// namespace processing

#endif  // FORX_PAN_H


