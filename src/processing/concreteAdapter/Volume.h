/*
 * ===========================================================================================================
 * Volume.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_VOLUME_H
#define FORX_VOLUME_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/Serialization.h"
#include "processing/dspTools.h"
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <processing/ModelFactory.hpp>

namespace processing {
using namespace parameter;
//============================================================================================================
/**
 * @class Volume.
 * Multipliziert Eingangs-Samplemenge mit Faktor.
 */
class Volume :
//============================================================================================================
public ProcessAdapter, 
public HasParameter, 
public com::Serializable
{
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<Volume> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Initalisiert Listener.
	 */
	void initListener() {
		volume->addValueChangedListener (
			boost::bind(&Volume::valueChanged, this, _1, _2)
		);
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert Volume-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & boost::serialization::base_object< ProcessAdapter > ( *this );
		ar & volume;
		if ( Archive::is_loading::value ) {
			frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
			if (!hI) {
				SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
					"Hostinfo == NULL"
				);
			}
			initListener();
			fader.setDuration( getFaderDuration( hI->getSampleRate() ) );  
			fader.setValue ( *volume );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	Volume (){} // wird nur von boost::serial. benutzt
	//--------------------------------------------------------------------------------------------------------
	FadeValue fader;
protected:
	//--------------------------------------------------------------------------------------------------------
	static float getFaderDuration( float sampleRate ) {
		float one_ms = sampleRate / 1000.0f;
		return one_ms*5.0f;
	}
	//--------------------------------------------------------------------------------------------------------
	Volume ( frx::processing::IHostInfo::Ptr hostInfo, float initValue = 1.0 ) : 
		ProcessAdapter( hostInfo, 1, 1 ), fader(initValue) 
	{
		setName ("Volume");
		volume = Parameter::create();
		volume->setName ("Volume");
		*volume = initValue;
		getInputNode(0)->setName ("Volume Input Node");
		getOutputNode(0)->setName ("Volume Output Node");
		fader.setDuration( getFaderDuration( hostInfo->getSampleRate() ) );  
	}
	//--------------------------------------------------------------------------------------------------------
	//Lautstaerke-wert
	Parameter::Ptr volume;
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @param initValue
	 * @return neues Volume-Objekt
	 */
	static Ptr create( frx::processing::IHostInfo::Ptr hostInfo ) {
		Ptr neu( new Volume(hostInfo, 1.0f ) );
		neu->self = neu;
		neu->initListener();
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Volume-Parameter geaendert.
	 * @param src
	 * @param value
	 */
	virtual void valueChanged ( void *src, const float &value ) { fader = value; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * HostInfo(Samplerate/Blocksize) geaendert.
	 */
	virtual void hostBaseConfigChanged() {
		frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
		if (!hI) {
			SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
				"Hostinfo == NULL"
			);
		}
		fader.setDuration( getFaderDuration( hI->getSampleRate() ) );  
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge des Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu verarbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @param index
	 * @return ausschliesslich Volumeparameter, da einzger Parameter.
	 */
	virtual Parameter::Ptr getParameter ( size_t index = 0 ) const { return volume; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @return 1
	 */
	virtual size_t getNumParameter () const { return 1; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~Volume () {
	}
};

FRX_MODELFACTORY_REGISTER(internal, Volume);

}// namespace processing

#endif  // FORX_VOLUME_H


