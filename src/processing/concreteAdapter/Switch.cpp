                                        /*
 * ===========================================================================================================
 * Switch.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include <boost/bind.hpp>
#include "Switch.h"


namespace processing{
//============================================================================================================
// Klasse Switch:
// verwaltet N FadeValue und dazugehoerige Parameter
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
const std::string Switch::STATE_GROUP_NAME = "State";
//------------------------------------------------------------------------------------------------------------
Switch::Switch( size_t _numStates, float sampleRate ) :
numStates(0), // wird ueber _addState erhoet
state(0),
nFader( FadeValues(_numStates) ),
nDurationIN( Parameters(_numStates) ),
nDurationOUT( Parameters(_numStates) ),
nCurveTypeIN( Parameters(_numStates) ),
nCurveTypeOUT( Parameters(_numStates) )
{
	for ( size_t i=0; i<_numStates; ++i ) {
		_addState();
	}
	setSampleRate( sampleRate ); 
}
//------------------------------------------------------------------------------------------------------------
void Switch::_addState() {
	Parameter::ParameterListenerFunction dI = boost::bind( 
		&Switch::durationINChanged, this, _1, _2 
	);
	Parameter::ParameterListenerFunction dO = boost::bind( 
		&Switch::durationOUTChanged, this, _1, _2 
	);
	Parameter::ParameterListenerFunction cT = boost::bind( 
		&Switch::curveTypeChanged, this, _1, _2 
	);
	State i = getNumStates();
	++numStates;
	// prepare duration parameter
	nDurationIN[i] = Parameter::create(i);
	parameterMap.push_back( nDurationIN[i] );
	nDurationIN[i]->setName("fade-in duration " + com::MyString(i+1) );
	nDurationIN[i]->setGroupName(STATE_GROUP_NAME + com::MyString(i+1));
	nDurationIN[i]->setLabel("ms");
	nDurationIN[i]->setMin(FLT_MIN);
	nDurationIN[i]->addValueChangedListener ( dI );
	nDurationOUT[i] = Parameter::create(i);
	parameterMap.push_back( nDurationOUT[i] );
	nDurationOUT[i]->setName("fade-out duration " + com::MyString(i+1) );
	nDurationOUT[i]->setGroupName(STATE_GROUP_NAME + com::MyString(i+1));
	nDurationOUT[i]->setLabel("ms");
	nDurationOUT[i]->setMin(FLT_MIN);
	nDurationIN[i]->addValueChangedListener ( dO );
	// prepare curve type parameter
	nCurveTypeIN[i] = Parameter::create(i);
	parameterMap.push_back( nCurveTypeIN[i] );
	nCurveTypeIN[i]->setName("fade-in curve type " + com::MyString(i+1) );
	nCurveTypeIN[i]->setGroupName(STATE_GROUP_NAME + com::MyString(i+1));
	nCurveTypeIN[i]->addValueChangedListener ( cT );
	nCurveTypeOUT[i] = Parameter::create(i);
	parameterMap.push_back( nCurveTypeOUT[i] );
	nCurveTypeOUT[i]->setName("fade-out curve type " + com::MyString(i+1) );
	nCurveTypeOUT[i]->setGroupName(STATE_GROUP_NAME + com::MyString(i+1));
	nCurveTypeOUT[i]->addValueChangedListener ( cT );
	// init
	*nDurationIN[i] = 0.01f;
	*nDurationOUT[i] = 0.01f;
	*nCurveTypeIN[i] = 0.0f;
	*nCurveTypeOUT[i] = 0.0f;
}
//------------------------------------------------------------------------------------------------------------
void Switch::addState() {
	nFader.push_back ( FadeValue() );
	nDurationIN.push_back ( Parameter::Ptr() );
	nDurationOUT.push_back ( Parameter::Ptr() );
	nCurveTypeIN.push_back ( Parameter::Ptr() );
	nCurveTypeOUT.push_back ( Parameter::Ptr() );
	_addState();
}
//------------------------------------------------------------------------------------------------------------
void Switch::durationINChanged ( void *src, const float &v ) {
	Parameter *p = (Parameter*)src;
	p->setDisplay(com::MyString(v*1000.0));
}
//------------------------------------------------------------------------------------------------------------
void Switch::durationOUTChanged ( void *src, const float &v ) {
	Parameter *p = (Parameter*)src;
	p->setDisplay(com::MyString(v*1000.0));
}
//------------------------------------------------------------------------------------------------------------
void Switch::curveTypeChanged ( void *src, const float &v ) {
	Parameter *p = (Parameter*) src;
	int t = mapInteger ( *p, FadeValue::NUM_FADE_TYPES );
	p->setDisplay ("type " + com::MyString(t+1) );
}
//------------------------------------------------------------------------------------------------------------
Switch::~Switch() {
}
//------------------------------------------------------------------------------------------------------------
void Switch::setState ( State x ) {
	State old = state;
	setFaderValue(state, 0.0f);
	state = x % numStates; 
	setFaderValue(state, 1.0f);
	if (old!=state) {
		stateChanged(old, state);
	}
}
}// namespace processing
