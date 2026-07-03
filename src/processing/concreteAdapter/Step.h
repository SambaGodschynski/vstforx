/*
 * ===========================================================================================================
 * Step.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_STEP_H
#define FORX_STEP_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/SerializationFwd.h"
#include "processing/dspTools.h"
#include "Switch.h"
#include "ValueTranslator.h"
#include <boost/function.hpp>

namespace processing {
using namespace parameter;
//============================================================================================================
/**
 * Klasse Step:
 * Erweitert Switch um Step-funktionen, d.h. rythmisches Schalten von States.
 * Jedes Step-State hatt eine dauer von N-Samples, diese wird ueber ein ValueTranslator-Objekt ermittelt.
 *
 * Oberklasse fuer Input/OutputStep
 */
class Step : public Switch {
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef FadeValue FactorType;
private:
	/**
	 * (De)Serialisiert Step-Objekt
	 * @param ar
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & boost::serialization::base_object<Switch> ( *this );
		ar & currTranslator;
		ar & steps;
		ar & duration;
		ar & nDuration;
        if (version>0) {
            ar & stepIndicator;
        }
		//:::::::::::::::IF_LOADING::::::::::::::::::::::::::::::::
		if ( !Archive::is_loading::value ) return;
		auto f = [this](void* s, const com::VstNumber& e){ durationParameterChanged(s, e); };
		for (int i=0; i<steps; ++i) {
			nDuration[i]->addValueChangedListener ( f );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	Step() {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * aktuelles ValueTranslator-Objekt @see ValueTranslator
	 */
	ValueTranslator *currTranslator;
	//--------------------------------------------------------------------------------------------------------
	int steps;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Step-Dauer in Samples
	 */
	int duration;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * StepDauer-Parameter pro StepState
	 */
	std::vector<Parameter::Ptr> nDuration;
    //--------------------------------------------------------------------------------------------------------
    Parameter::Ptr stepIndicator;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * StepDauer-Parameter geandert
	 * @param src
	 * @param v neuer Parameter-Wert
	 */
	void durationParameterChanged ( void *src, const float &v );
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::function<void(State, State)> StateChangedDelegate;
	StateChangedDelegate stateChangedDelegate;
	//--------------------------------------------------------------------------------------------------------
	virtual void stateChanged(State old, State _new);
	//--------------------------------------------------------------------------------------------------------
	/**
	 * holt Step-Dauer aus ValueTranslator und setzt uebernimmt diese
	 */
	void resetDuration();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * verringert Step-Dauer um 1 Sample.
	 * Ist dauer == 0, schaltet naechsten Step.
	 */
	void skimDuration() { 
		if ( duration-- <= 0 ) {
			nextStep();
			resetDuration();
		}
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt aktuelles ValueTranslator-Objekt
	 * @param tr
	 */
	void setValueTranslator ( ValueTranslator *tr ){ currTranslator = tr; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt alle Parameterwerte neu (TODO: wozu?).
	 */
	void resetParameterLabel();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * fuegt neuen Step-State hinzu
	 */
	virtual void addState();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return aktuelles ValueTranslator-Objekt
	 */
	ValueTranslator * getValueTranslator (){ return currTranslator; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Step-Parameter zu index. Wirft std::out_of_range
	 */
	Parameter::Ptr getParameter ( size_t index );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Step-Parameter zu index. Wirft std::out_of_range
	 */
	Parameter::Ptr getStepIndicator () const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * verschiebung der Step-Dauer um N Samples
	 * @param off N
	 */
	void setOffset ( int off )  { duration+=off; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Beeinflusst Switch::Fader
	 * @param sampleRate
	 */
	void setSampleRate( float sampleRate ) {
		Switch::setSampleRate( sampleRate );
	}
	//--------------------------------------------------------------------------------------------------------
	Step( ValueTranslator*, size_t initSteps = 2, float sampleRate = 0.0f );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return aktuellen Step-State
	 */
	size_t getState() const { return Switch::getState(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Step-Reset
	 */
	void reset(){
		setFaderValue(getState(), 0.0f);
		setState (0);
		resetDuration();
		setFaderValue(getState(), 1.0f);
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Schalet naechsten Step-State
	 */
	void nextStep(){ setState ( ( getState() + 1 ) % getNumSteps() ); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Setzt Step-State anzahl TODO: make private
	 * @param n
	 */
	void setNumSteps( int n ) { steps = n; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return anzahl aller Step-States
	 */
	int getNumSteps() { return steps; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~Step();
};
}// namespace processing

BOOST_CLASS_VERSION(processing::Step, 1)


#endif  // FORX_STEP_H


