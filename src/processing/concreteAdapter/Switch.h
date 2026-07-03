/*
 * ===========================================================================================================
 * Switch.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_SWITCH_H
#define FORX_SWITCH_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/SerializationFwd.h"
#include "processing/dspTools.h"

namespace processing {
using namespace parameter;
//============================================================================================================
/**
 * @class Switch.
 * Oberklasse fuer Input/OutputSwitch, Step. Vewaltet N FadeValues
 * (fadeIn/fadeOut pro State)
 */
class Switch : public HasParameter
{
//============================================================================================================
	friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef size_t State;
	//--------------------------------------------------------------------------------------------------------
	typedef std::vector<FadeValue> FadeValues;
	//--------------------------------------------------------------------------------------------------------
	typedef std::vector<Parameter::Ptr> Parameters;
	//--------------------------------------------------------------------------------------------------------
	static const std::string STATE_GROUP_NAME;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert Switch-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & numStates;
		ar & nFader;
		ar & nDurationIN;
		ar & nDurationOUT;
		ar & nCurveTypeIN;
		ar & nCurveTypeOUT;
		ar & parameterMap;
		ar & state;
		if ( Archive::is_loading::value ) { // rebind func listener
			auto dI = [this](void* s, const com::VstNumber& e){ durationINChanged(s, e); };
			auto dO = [this](void* s, const com::VstNumber& e){ durationOUTChanged(s, e); };
			auto cT = [this](void* s, const com::VstNumber& e){ curveTypeChanged(s, e); };

			for ( size_t i=0; i<numStates; ++i ) {
				nDurationIN[i]->addValueChangedListener ( dI );
				nDurationOUT[i]->addValueChangedListener ( dO );
				nCurveTypeIN[i]->addValueChangedListener ( cT );
				nCurveTypeOUT[i]->addValueChangedListener ( cT );
			}
		}
	}
	//--------------------------------------------------------------------------------------------------------
	size_t numStates;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Anzahl der Parameter pro State
	 */
	enum { NUM_P = 4 };
	//--------------------------------------------------------------------------------------------------------
	/**
	 * aktueller State
	 */
	State state;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Parameter durationIn geandert
	 * @param src
	 * @param v neuer Wert
	 */
	void durationINChanged ( void *src, const float &v );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Parameter durationOut geandert
	 * @param src
	 * @param v neuer Wert
	 */
	void durationOUTChanged ( void *src, const float &v );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Parameter curveType geandert
	 * @param src
	 * @param v neuer Wert
	 */
	void curveTypeChanged ( void *src, const float &v );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Anzahl der Samples einer Ms.
	 */
	float one_ms_in_samples;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * FadeValues
	 */
	FadeValues nFader;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Parameter Container: Parameter pro State
	 */
	Parameters nDurationIN, nDurationOUT, nCurveTypeIN, nCurveTypeOUT, parameterMap; 
	//--------------------------------------------------------------------------------------------------------
	void _addState();
protected:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Setzt neuen Fader-Zielwert
	 * @param n State
	 * @param v neuer Wert
	 */
	void setFaderValue ( State n, const FadeValue::T &v ) {
		bool out = v<nFader[n];
		float d = out ? *nDurationOUT[n] : *nDurationIN[n]; // fadein oder fadeout?
		int t = mapInteger ( ( out ? *nCurveTypeOUT[n] : *nCurveTypeIN[n] ), FadeValue::NUM_FADE_TYPES );
		nFader[n].setDuration ( one_ms_in_samples * 1000.0f * d);
		nFader[n].setType ( (FadeValue::FadeType) t );
		nFader[n].setValue(v);
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void stateChanged(State old, State _new) = 0; 
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Setzt Samplerate. Beeinflusst Fader.
	 * @param sampleRate
	 */
	void setSampleRate ( float sampleRate ) {
		one_ms_in_samples = sampleRate / 1000.0f;
	}
	//--------------------------------------------------------------------------------------------------------
	Switch( size_t numStates = 0, float sampleRate = 0.0f );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Setzt State.
	 * @param x
	 */
	void setState ( State x );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * fuegt State hinzu.
	 */
	virtual void addState();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Schaltet naechsten State
	 */
	void nextState() { setState( state + 1 ); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Schaltet vorherrigen State
	 */
	void prevState() { setState( state - 1 ); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Aktueller State
	 */
	State getState() const { return state; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl States
	 */
	size_t getNumStates() const { return numStates; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~Switch();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Parameter zu index. Wirft std::out_of_range.
	 */
	virtual Parameter::Ptr getParameter ( size_t index ) const {  return parameterMap.at(index); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param state
	 * @return DurationParameterIN zu State. Wirft std::out_of_range.
	 */
	Parameter::Ptr getDurationParameterIN ( State state ) const {  return nDurationIN.at(state); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param state
	 * @return DurationParameterOUT zu State. Wirft std::out_of_range.
	 */
	Parameter::Ptr getDurationParameterOUT ( State state ) const {  return nDurationOUT.at(state); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param state
	 * @return CurveTypeParameterIN zu State. Wirft std::out_of_range.
	 */
	Parameter::Ptr getCurveTypeParameterIN ( State state ) const {  return nCurveTypeIN.at(state); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param state
	 * @return CurveTypeParameterOUT zu State. Wirft std::out_of_range.
	 */
	Parameter::Ptr getCurveTypeParameterOUT ( State state ) const {  return nCurveTypeOUT.at(state); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller Switch-Parameter.
	 */
	virtual size_t getNumParameter () const { return NUM_P*numStates; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param n
	 * @return Fader-Wert und erhoert Fader-t um 1.
	 */
	FadeValue::T getFaderValueAndIncT ( size_t n ) { return nFader[n].getValue(); } 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param n
	 * @return Fader-Objekt
	 */
	FadeValue * getFader( State n ) { return &nFader[n]; }
};
}// namespace processing

#endif  // FORX_SWITCH_H


