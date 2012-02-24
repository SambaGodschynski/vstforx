/*
 * ===========================================================================================================
 * dspTools.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef DSP_TOOLS_H
#define DSP_TOOLS_H

#include <math.h>
#include <string>
#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/Serialization.h"
#include "time.h"
#include "processing/IHostInfo.h"
#include <boost/timer.hpp>
#include <map>

namespace processing {
	//====================================================================================================
	namespace musicalValues {
	//====================================================================================================
		//------------------------------------------------------------------------------------------------
		struct NoteLength { 
			std::string str;
			double val;
		};
		//------------------------------------------------------------------------------------------------
		extern const NoteLength noteLengthTable[];
		//------------------------------------------------------------------------------------------------
		extern std::string getCCName ( size_t index );
		//------------------------------------------------------------------------------------------------
		extern const int NUM_STDNOTES;
	} // namespace musicalValues
//--------------------------------------------------------------------------------------------------------
// wandelt double notenwert in samples (int) um.
inline int note2Sample ( double noteVal, double tempo, double samplerate ){
	double oneMsInSamples = samplerate / 1000.0;
	double wholeInMs = 4000.0 * 60.0 / tempo;
	return (int) ( oneMsInSamples * wholeInMs * noteVal );
}
//========================================================================================================
// Klasse SimpleFadeValue:
// Erreicht sein Zielwert nach N schritten. ( n Abfragen bzw. Samples )
//========================================================================================================
template <typename T, int N> 
class SimpleFadeValue {
private:
	//----------------------------------------------------------------------------------------------------
	T v,step,dst;
	//----------------------------------------------------------------------------------------------------
	bool up;
public:
	//----------------------------------------------------------------------------------------------------
	SimpleFadeValue () : v(0), dst(0) {}
	//----------------------------------------------------------------------------------------------------
	operator T(){ return getValue(); }
	//----------------------------------------------------------------------------------------------------
	T & getValue() {
		if (up &&  v>=dst) return dst;
		if (!up && v<=dst) return dst;
		v+=step;
		return v;
	}
	//----------------------------------------------------------------------------------------------------
	void operator=( const T &nV ){
		if ( nV > v ) up = true;
		else up = false;
		dst = nV;
		step = (nV - v)/N;
	}
};
//========================================================================================================
template <typename T>
class SimpleFadeValue<T,0> {};
//========================================================================================================
// Namespace Clockedge :
// Signal Flanke. Ist einmalig HIGH bzw. LOW bei einem 
// Schaltvorgang von Positive ( 0 nach 1 ) / Negative( 1 nach 0 ) ansonsten 0.
//========================================================================================================
class ClockEdge {
friend class boost::serialization::access;
private:
	//---------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){ ar & fl; }
	//------------------------------------------------------------------------------------------------
	bool fl;
public:
	enum EdgeValue { NONE, HIGH, LOW };  
	//------------------------------------------------------------------------------------------------
	ClockEdge() : fl(false) {}
	//------------------------------------------------------------------------------------------------
	EdgeValue in ( bool i ) {
		if ( fl != i ){ fl = i; return i ? HIGH : LOW; }
		return NONE;
	}
};
//========================================================================================================
// Klasse Timer:
// isElapsed() hatt wahrheitswert true nach abgelaufener zeit (s).
//========================================================================================================
class Timer {
private:
	//----------------------------------------------------------------------------------------------------
	boost::timer timer;
	//----------------------------------------------------------------------------------------------------
	bool _isRunning;
public:
	//----------------------------------------------------------------------------------------------------
	Timer () : _isRunning(false) {}
	//----------------------------------------------------------------------------------------------------
	void stop () { _isRunning=false; } 
	//----------------------------------------------------------------------------------------------------
	void start () { _isRunning=true; timer.restart(); } 
	//----------------------------------------------------------------------------------------------------
	bool isElapsed( const double &t ) const { return timer.elapsed() > t && _isRunning; } 
	//----------------------------------------------------------------------------------------------------
	bool isRunning() const { return _isRunning; }
};
//========================================================================================================
// DelayedClockEdge:
// kombination aus EdgeValue und Timer.
// in: boolescher ausdruck
// out: HIGH/LOW/NONE 
// ausgabe verzoegert um t in sec.
//
//     +-------------+
//     |             |
//  in |...|...|...|...|...|...|...|...|
//     o----t---->+----t---->+          sec
//                |          |
//       flanke HIGH    fl. LOW
//========================================================================================================
class DelayedClockEdge {
private: // h_ HIGH / l_LOW
	//----------------------------------------------------------------------------------------------------
	ClockEdge h_clockEdge, l_clockEdge, out_clockEdge;
	//----------------------------------------------------------------------------------------------------
	Timer h_timer, l_timer;
	//----------------------------------------------------------------------------------------------------
	double duration;
	//----------------------------------------------------------------------------------------------------
	bool out;
	//----------------------------------------------------------------------------------------------------
	void h_in( bool expr ) {
		// start/stop timer
		if ( expr && !h_timer.isRunning() ) h_timer.start();
		else if ( !expr && h_timer.isRunning() ) h_timer.stop(); 
		// flanke: timer
		ClockEdge::EdgeValue timer_elapsed = h_clockEdge.in( h_timer.isElapsed( duration ) );
		if ( timer_elapsed == ClockEdge::HIGH ) { 
			out = true;
		}
	}
	//----------------------------------------------------------------------------------------------------
	void l_in( bool expr ) {
		// start/stop timer
		if ( !expr && !l_timer.isRunning() ) l_timer.start();
		else if ( expr && l_timer.isRunning() ) l_timer.stop(); 
		// flanke: timer
		ClockEdge::EdgeValue timer_elapsed = l_clockEdge.in( l_timer.isElapsed( duration ) );
		if ( timer_elapsed == ClockEdge::HIGH ) {
			out = false;
		}
	}
public:
	DelayedClockEdge ( const double &duration_sec ) : duration( duration_sec ), out(false) {}
	//----------------------------------------------------------------------------------------------------
	ClockEdge::EdgeValue in ( bool expr ) {
		h_in ( expr );
		l_in ( expr );
		return out_clockEdge.in ( out );
	}
};
//========================================================================================================
// Abstrakte Klasse AFadeValue:
// Erreicht sein Zielwert nach d mal getValue() abfragen.
//========================================================================================================
class AFadeValue {
friend class boost::serialization::access;
public:
	//----------------------------------------------------------------------------------------------------
	typedef float T;
private:
	//----------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & b;
		ar & e;
		ar & d;
		ar & c;
		ar & t;
		ar & value;
		ar & fin;
	}
protected:
	//----------------------------------------------------------------------------------------------------
	T b,e,d,c,t,value; // b = anfangswert, e = endwert, d = dauer (in samples), c = e - b
	//----------------------------------------------------------------------------------------------------
	virtual const T & _getValue() = 0;
	//----------------------------------------------------------------------------------------------------
	explicit AFadeValue ( const T &initvalue = 0 ) : 
	t(0), b(0), e(0), d(1), c(0), value(initvalue), fin(false) {} 
	//----------------------------------------------------------------------------------------------------
	bool fin;
public:
	//----------------------------------------------------------------------------------------------------
	void clone ( const AFadeValue &n ){
		b = n.b;
		e = n.e;
		c = n.c;
		t = n.t;
		fin = n.fin;
		value = n.value;
	}
	//----------------------------------------------------------------------------------------------------
	operator T() { return getValue(); }
	//----------------------------------------------------------------------------------------------------
	bool isFinished(){ return fin; }
	//----------------------------------------------------------------------------------------------------
	const T & getValue() { 
		if ( t > d )  { fin=true;return e; }
		fin = false;
		++t; 
		return _getValue(); 
	}
	//----------------------------------------------------------------------------------------------------
	void setValue ( const T &v ){
		b = value;
		e = v;
		c = e - b;
		t = 0;
		fin = false;
	}
	//----------------------------------------------------------------------------------------------------
	void resetEndValue ( const T &v ){
		b = value;
		e = v;
		c = e - b;
	}
	//----------------------------------------------------------------------------------------------------
	float getElapsedTime () { return t; }
	//----------------------------------------------------------------------------------------------------
	void setDuration ( const T &v ){ d = (v>1.0f)? v : 1.0f; } // darf nicht 0 sein.
	//----------------------------------------------------------------------------------------------------
	void operator = ( const T &v ) { setValue (v); }
	//----------------------------------------------------------------------------------------------------
	virtual ~AFadeValue(){}
};
//========================================================================================================
// Klasse FadeValue:
// Berechnungen nach Robert Penner: Motion, Tweening, and Easing.
// http://www.robertpenner.com/easing/penner_chapter7_tweening.pdf
//========================================================================================================
class FadeValue : public AFadeValue {
friend class boost::serialization::access;
public:
	//----------------------------------------------------------------------------------------------------
	enum FadeType { LIN, QUAD, CUB, QUART, QUINT, EXP, NUM_FADE_TYPES };
private:
	//----------------------------------------------------------------------------------------------------
	FadeType type;
	//----------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< AFadeValue > ( *this );
		ar & p;
		ar & type;
	}
	//----------------------------------------------------------------------------------------------------
	T p;
public:
	//----------------------------------------------------------------------------------------------------
	explicit FadeValue ( const T &initValue = 0 ) : AFadeValue( initValue ), type (LIN) {
		setDuration ( 1.0f );
	} 
	//----------------------------------------------------------------------------------------------------
	void setType ( FadeType _type ) { type = _type; }
	//----------------------------------------------------------------------------------------------------
	FadeType getType () { return type; }
	//----------------------------------------------------------------------------------------------------
	virtual const T & _getValue() {
		switch ( type ){
			case LIN : return calcLIN(); 
			case QUAD : return calcQUAD();
			case CUB : return calcCUB(); 
			case QUART : return calcQUART();
			case QUINT : return calcQUINT();
			case EXP : return calcEXP();
		}
		return calcLIN();
	}
	//----------------------------------------------------------------------------------------------------
	const T & calcLIN(){ value = c*t/d + b; return value; }
	//----------------------------------------------------------------------------------------------------
	const T & calcQUAD(){ p=t/d; value = c*p*p + b; return value; }
	//----------------------------------------------------------------------------------------------------
	const T & calcCUB(){ p=t/d; value = c*p*p*p + b; return value; }
	//----------------------------------------------------------------------------------------------------
	const T & calcQUART(){ p=t/d; value = c*p*p*p*p + b; return value; }
	//----------------------------------------------------------------------------------------------------
	const T & calcQUINT(){ p=t/d; value = c*p*p*p*p*p + b; return value; }
	//----------------------------------------------------------------------------------------------------
	const T & calcEXP(){ p=t/d; value = c * (float)pow(2.0, 10.0 * ( p - 1.0 )) + b; return value; }
	//----------------------------------------------------------------------------------------------------
	void operator = ( const T &v ) { setValue (v); }
};
//========================================================================================================
// Klasse ADSR :
// Attack, Decay, Sustain, Release bestehend aus FadeValue
//========================================================================================================
class ADSR : public processing::parameter::HasParameter {
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//----------------------------------------------------------------------------------------------------
	enum States { A, D, S, R, NUM_STATES };
	//----------------------------------------------------------------------------------------------------
	States getState() { return (States)state; }
private:
	//----------------------------------------------------------------------------------------------------
	int holdSamples;
	//----------------------------------------------------------------------------------------------------
	void save ( oArchive& ar, const unsigned int version ) const; 
	//----------------------------------------------------------------------------------------------------
	void load ( iArchive& ar, const unsigned int version ); 
	//----------------------------------------------------------------------------------------------------
	IHostInfo *hostInfo;
	//----------------------------------------------------------------------------------------------------
	int state;
	//----------------------------------------------------------------------------------------------------
	processing::parameter::Parameter::Ptr duration[NUM_STATES],
				   level[NUM_STATES], 
		           curveType[NUM_STATES], 
			       threshold, 
			       hold, 
			       mode; // absolut / relativ ; bei relative wird adsr-out mit input-value mult.
	//----------------------------------------------------------------------------------------------------
	FadeValue fader;
	//----------------------------------------------------------------------------------------------------
	static const float HOLD_FAK;
	//----------------------------------------------------------------------------------------------------
	enum { FADER_TYPES = FadeValue::NUM_FADE_TYPES };
	//----------------------------------------------------------------------------------------------------
	vector<processing::parameter::Parameter::Ptr> parameterMap;
	//----------------------------------------------------------------------------------------------------
	int blockSize;
	//----------------------------------------------------------------------------------------------------
	float sampleRate;
	//----------------------------------------------------------------------------------------------------
	float in, value, attackVal /* der wert der zum attack anlag */; 
	//----------------------------------------------------------------------------------------------------
	float maxDurationInSec; // trigger schwelle
	//----------------------------------------------------------------------------------------------------
	ClockEdge trigger;
	//----------------------------------------------------------------------------------------------------
	ClockEdge fadeFinished;
	//----------------------------------------------------------------------------------------------------
	void curveTypeChanged ( void *src, const float &v );
	//----------------------------------------------------------------------------------------------------
	void levelChanged ( void *src, const float &v );
	//----------------------------------------------------------------------------------------------------
	void durationChanged ( void *src,  const float &v );
	//----------------------------------------------------------------------------------------------------
	void holdChanged ( void *src,  const float &v );
	//----------------------------------------------------------------------------------------------------
	void modeChanged ( void *src, const float &v );
	//----------------------------------------------------------------------------------------------------
	void initFaderTypes();
	//----------------------------------------------------------------------------------------------------
	ADSR () : maxDurationInSec(0) {}
	//----------------------------------------------------------------------------------------------------
	void nextState() { 
		if (state!=S) setState ( (state+1)%NUM_STATES );
	}
	//----------------------------------------------------------------------------------------------------
	void setState ( int state ){
		ADSR::state = state;
		fader.setDuration ( sampleRate * *duration[state] * maxDurationInSec ); 
		fader = *level[state] * ( isModeAbsolute() ? 1.0f : attackVal );
	}
	//----------------------------------------------------------------------------------------------------
	void release() { setState (R); }
public:
	//----------------------------------------------------------------------------------------------------
	void hostInfoChanged() {
		blockSize = hostInfo->getBlockSize();
		sampleRate = hostInfo->getSampleRate();
	}
	//----------------------------------------------------------------------------------------------------
	bool isModeAbsolute() const  { return *mode < 0.5f; }
	//----------------------------------------------------------------------------------------------------
	ADSR (IHostInfo *hostInfo, float maxDurationInSec = 1.0f );
	//----------------------------------------------------------------------------------------------------
	virtual ~ADSR();
	//----------------------------------------------------------------------------------------------------
	void setInput ( float v  ){
		value = v;
		ClockEdge::EdgeValue e = trigger.in ( v > *threshold || holdSamples > 0 );
		switch ( e ) {
			case ClockEdge::HIGH :
				nextState();
				holdSamples = static_cast<int>( sampleRate * *hold * HOLD_FAK );
				break;
			case ClockEdge::LOW :
				release();
				break;
			default :
				holdSamples -= blockSize;
		}	
	}
	//----------------------------------------------------------------------------------------------------
	// increments faders t value and returns fader value of t. 
	float process (){
		if ( state == R ) return fader;
		ClockEdge::EdgeValue n = fadeFinished.in ( fader.isFinished() );
		if ( n == ClockEdge::HIGH ) nextState();
		return fader;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual processing::parameter::Parameter::Ptr getParameter ( size_t nr = 0 ) const { 
		if ( nr > parameterMap.size() ) return processing::parameter::Parameter::Ptr();
		return parameterMap.at (nr);
	}
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const { return parameterMap.size(); }
};



} //namespace com

#endif