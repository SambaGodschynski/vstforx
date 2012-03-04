/*
 * ===========================================================================================================
 * parameter.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include <limits> 
#include "parameter.h" 

namespace processing {
namespace parameter {
//============================================================================================================
// Schnitstelle: ConnectionOperator.
//============================================================================================================
//============================================================================================================
// Klasse: ParameterConnection.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ParameterConnection::ParameterConnection(ParameterPtr a, ParameterPtr b) : a(a), b(b) {
	Parameter::ParameterListenerFunction aC = boost::bind( 
		&ParameterConnection::onChangedA, this, _1, _2 
	);
	Parameter::ParameterListenerFunction bC = boost::bind( 
		&ParameterConnection::onChangedA, this, _1, _2 
	);
	a->addValueChangedListenerF(aC);
	b->addValueChangedListenerF(aC);
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::onChangedA(void *src, const VstNumber &newValue) {
	VstNumber t = newValue;
	BOOST_FOREACH(ConnectionOperator::Ptr op, ops) {
		t = op->operate(t);
	}
	b->setValue(t);
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::onChangedB(void *src, const VstNumber &newValue) {
	VstNumber t = newValue;
	BOOST_FOREACH(ConnectionOperator::Ptr op, ops) {
		t = op->operateInverse(t);
	}
	a->setValue(t);
}
//============================================================================================================
// Klasse: Parameter.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
Parameter::Parameter( int index ):
updateLock(false), 
_min(0.0f),
_max(1.0f)
{
	nr = instances++;
	Parameter::index = index;
	setName ("Parameter:" + MyString(nr) );
	setValue (0.0);
}

//------------------------------------------------------------------------------------------------------------
Parameter::~Parameter() {
	instances--;
}

//------------------------------------------------------------------------------------------------------------
int Parameter::instances = 0;

} // namespace parameter
} // namespace processing



