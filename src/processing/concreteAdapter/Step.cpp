                                        /*
 * ===========================================================================================================
 * Step.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "Step.h"


namespace processing{
//============================================================================================================
// Klasse Step:
// Verwaltet Step zustaende und zugehoerige Multiplikations Faktoren.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
Step::Step( ValueTranslator* tr, size_t initSteps, float sampleRate ) : 
currTranslator(tr), Switch (initSteps, sampleRate ),  steps(initSteps), nDuration(initSteps) 
{ 
	Parameter::ParameterListenerFunction f = boost::bind( 
			&Step::durationParameterChanged, this, _1, _2 
		);
	for (int i=0; i<steps; ++i) {
		setFaderValue ( i, 0.0f );
		nDuration[i] = Parameter::create(); 
		Parameter::Ptr p = getParameter(i);
		p->setName ( "Step " + com::MyString(i+1) + " duration." );
		p->setGroupName(STATE_GROUP_NAME + com::MyString(i+1));
		p->addValueChangedListener (f);
		p->setValue(0.35f);
	}
    stepIndicator = Parameter::create();
    stepIndicator->setName("current step");
	resetDuration();
}
//------------------------------------------------------------------------------------------------------------
Step::~Step() {
}
//------------------------------------------------------------------------------------------------------------
void Step::addState() {
	Switch::addState();
	
	Parameter::ParameterListenerFunction f = boost::bind( 
		&Step::durationParameterChanged, this, _1, _2 
	);
	nDuration.push_back ( Parameter::Ptr() );
	size_t i = getNumStates() - 1;
	nDuration[i] = Parameter::create(); 
	Parameter::Ptr p = getParameter(i);
	p->setName ( "Step " + com::MyString(i+1) + " duration." );
	p->setGroupName(STATE_GROUP_NAME + com::MyString(i+1));
	p->addValueChangedListener (f);
	p->setValue(0.35f);

}
//------------------------------------------------------------------------------------------------------------
void Step::resetParameterLabel(){
	for (int i=0; i<steps; ++i) {
		Parameter::Ptr p = getParameter(i);
		p->setValue(p->getValue());
	}
}
//------------------------------------------------------------------------------------------------------------
void Step::resetDuration() {
	duration = currTranslator->translate ( *nDuration[getState()] );
}
//------------------------------------------------------------------------------------------------------------
void Step::durationParameterChanged ( void *src, const float &v ){
	Parameter *p = (Parameter*) src;
	p->setDisplay ( currTranslator->translateAsString(v) );
}
//------------------------------------------------------------------------------------------------------------
void Step::stateChanged(State old, State _new) {
    if (stateChangedDelegate) {
        stateChangedDelegate(old, _new);
    }
    // update step indicator
    stepIndicator->setValue(_new/(float)(getNumStates()-1));
    stepIndicator->setDisplay("step " + com::MyString(_new+1));
}
//------------------------------------------------------------------------------------------------------------
parameter::Parameter::Ptr Step::getParameter ( size_t index ) {
    return nDuration.at(index);
}
//------------------------------------------------------------------------------------------------------------
parameter::Parameter::Ptr Step::getStepIndicator () const {
    return stepIndicator;
}
}// namespace processing
