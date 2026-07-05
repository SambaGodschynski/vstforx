/*
 * ===========================================================================================================
 * OutputStep.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_OUTPUTSTEP_H
#define FORX_OUTPUTSTEP_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/SerializationFwd.h"
#include "com/one4All.h"
#include "Step.h"
#include "FixTimeTranslator.h"
#include "SyncTranslator.h"
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>

namespace processing {
//============================================================================================================
/**
 * @class OutputStep.
 * Schaltet Eingangs-Samplemengen auf N Ausgaenge.
 */
class OutputStep: 
public ProcessAdapter, 
public parameter::HasParameter, 
public com::Serializable, 
public VariableOutputAdapter,
public com::IHasState
{
//============================================================================================================
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef std::shared_ptr<OutputStep> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisiert OutputStep-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void save ( com::oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Deserialisiert OutputStep-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void load ( com::iArchive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	OutputStep() : fixTimeValue(0.0f, 0.0f) {};
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * blockt hinzufuegen von OutputNode gegen processAdapter()
	 */
	com::Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Flankenschalter auf Host-Transport (@see VST-SDK VstTimeInfo)
	 */
	ClockEdge transport;
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr type;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * initalisiert OutputStep
	 */
	void init();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * wird aufgerufen wenn Host-Transport sich aendert (play gedureckt)
	 * Setzt Step-Dauer verschiebung, um die naechste 1/4 Note zu treffen.
	 */
	inline void reset();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Step-Typ geandert (Sync/Fixed)
	 * @param src
	 * @param v
	 */
	void typeChanged ( void *src, const float& v );
	//--------------------------------------------------------------------------------------------------------
	typedef std::vector<Frames*> OutputMatrix;
	//--------------------------------------------------------------------------------------------------------
	OutputMatrix outpMatrix;
	//--------------------------------------------------------------------------------------------------------
	inline void processFrames ( Frames *iFrame, OutputMatrix &fr, Processor::Int numSamples );
protected:
	//--------------------------------------------------------------------------------------------------------
	virtual void stateChangedHandler(Switch::State old, Switch::State _new) {
		sce::EventSender<sce::PropertyChanged>::notifyListeners(
			this, 
			sce::PropertyChanged(PROPERTY_SWITCH_STATE, SwitchState(false,old), SwitchState(false,_new))
		);
	}
	//--------------------------------------------------------------------------------------------------------
	std::vector<Parameter::Ptr> parameterMap;
	//--------------------------------------------------------------------------------------------------------
	Step *cStep;
	//--------------------------------------------------------------------------------------------------------
	FixTimeValue fixTimeValue;
	//--------------------------------------------------------------------------------------------------------
	SyncTranslator *sync;
	//--------------------------------------------------------------------------------------------------------
	virtual void setState ( size_t ignore ) {} 
	//--------------------------------------------------------------------------------------------------------
	OutputStep( frx::processing::IHostInfo::Ptr hostInfo, int initSteps = 2 );
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @param initSteps
     * @param not used, needed to fit ModelFactory's create function signature
	 * @return neues OutputStep-Objekt
	 */
	static Ptr create( frx::processing::IHostInfo::Ptr hostInfo, int notUsed = 0, int initSteps=2 ) {
		Ptr neu( new OutputStep(hostInfo, initSteps) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * resetet SampleRate
	 */
	virtual void hostBaseConfigChanged() {
		frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
		if (!hI) {
			SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
				"Hostinfo == NULL"
			);
		}
		fixTimeValue.hostBaseConfigChanged( hI->getSampleRate() );
		if (sync) 
			sync->hostBaseConfigChanged();
		cStep->setSampleRate ( hI->getSampleRate() );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge der Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	virtual ~OutputStep();
	/**
	 * @param index
	 * @return liefert Parameter zu index. Wirft: std::out_of_range
	 */
	virtual Parameter::Ptr getParameter ( size_t index = 0 ) const {return parameterMap.at(index);}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return aktuellen State. Wenn inaktiv (@see ProcessorNode::isActive()) UINT_MAX
	 */
	virtual size_t getState() const { return aNode->isActive() ? cStep->getState() : UINT_MAX; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller OutputStep-Parameter
	 */
	virtual size_t getNumParameter () const { return parameterMap.size(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt neuen OutputNode hinzu.
	 * @return neues OutputNode-Objekt
	 */
	ProcessorNode::Ptr addOutputNode();
};

}// namespace processing

#endif  // FORX_OUTPUTSTEP_H


