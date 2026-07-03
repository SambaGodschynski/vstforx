/*
 * ===========================================================================================================
 * InputStep.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_INPUTSTEP_H
#define FORX_INPUTSTEP_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/SerializationFwd.h"
#include "com/one4All.h"
#include "Step.h"
#include "FixTimeTranslator.h"
#include "SyncTranslator.h"
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <processing/ModelFactory.hpp>

namespace processing {
using namespace parameter;
//============================================================================================================
/**
 * @class InputStep.
 * Schaltet Eingangs-Samplemengen auf N Ausgaenge.
 */
class InputStep: 
public ProcessAdapter, 
public HasParameter, 
public com::Serializable, 
public VariableInputAdapter,
public com::IHasState
{
//============================================================================================================
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<InputStep> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisiert InputStep-Objekt
	 * @param ar
	 * @param version
	 */
	void save ( com::oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Deserialisiert InputStep-Objekt
	 * @param ar
	 * @param version
	 */
	void load ( com::iArchive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	InputStep() : fixTimeValue(0.0f, 0.0f) {}
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * blockt hinzufuegen von InputNode gegen processAdapter()
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
	Frames tmpFrame;
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
	typedef std::vector<Frames*> InputMatrix;
	//--------------------------------------------------------------------------------------------------------
	InputMatrix inputMatrix;
	//--------------------------------------------------------------------------------------------------------
	inline void processFrames ( InputMatrix &fr, Processor::Int numSamples );
protected:
	//--------------------------------------------------------------------------------------------------------
	void stateChangedHandler(Switch::State old, Switch::State _new) {
		sce::EventSender<sce::PropertyChanged>::notifyListeners(
			this, 
			sce::PropertyChanged(PROPERTY_SWITCH_STATE, SwitchState(true,old), SwitchState(true,_new))
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
	InputStep( frx::processing::IHostInfo::Ptr hostInfo, int initSteps = 2 );
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @param initSteps
     * @param not used, needed to fit ModelFactory's create function signature
	 * @return neues InputStep-Objekt
	 */
	static Ptr create( frx::processing::IHostInfo::Ptr hostInfo, int initSteps = 2, int notUsed = 0)
    {
		Ptr neu( new InputStep(hostInfo, initSteps) );
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
		tmpFrame.setSize( hI->getBlockSize() );
		cStep->setSampleRate ( hI->getSampleRate() );
		if (sync) 
			sync->hostBaseConfigChanged();
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge der Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	virtual ~InputStep();
	//--------------------------------------------------------------------------------------------------------
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
	 * @return Anzahl aller InputStep-Parameter
	 */
	virtual size_t getNumParameter () const { return parameterMap.size(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt neuen InputNode hinzu.
	 * @return neues InputNode-Objekt
	 */
	ProcessorNode::Ptr addInputNode();
};
namespace {
    const bool INTERNAL_INSTEP_IO_Registered =
        frx::processing::ModelFactory::instance().
            registerWithIO<InputStep>("internal.InputStep", &InputStep::create);
    
    const bool INTERNAL_INSTEP_Registered =
        frx::processing::ModelFactory::instance().
		register_<InputStep>("internal.InputStep", boost::bind(&InputStep::create, _1, 2, 0));
}

}// namespace processing

#endif  // FORX_INPUTSTEP_H


