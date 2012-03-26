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
ParameterConnection::ParameterConnection(ParameterPtr a, ParameterPtr b) : 
	updateLock(false), a(a), b(b) 
{
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::initListener(ConnectionOperator::Ptr op) {
	HasParameter *hP = dynamic_cast<HasParameter*>(op.get());
	if (!hP)
		return;
	for (size_t i=0; i<hP->getNumParameter(); ++i) {
		hP->getParameter(i)->addTrackedValueChangedListener(
			boost::bind(&ParameterConnection::onOperatorParameterChanged, this, _1, _2),
			self
		);
	}
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::initListener() {
	a->addTrackedValueChangedListener( 
		boost::bind(&ParameterConnection::onChangedA, this, _1, _2),
		self
	);
	b->addTrackedValueChangedListener(
		boost::bind(&ParameterConnection::onChangedB, this, _1, _2 ),
		self
	);
	BOOST_FOREACH(ConnectionOperator::Ptr op, ops) {
		initListener(op);
	}
}
//------------------------------------------------------------------------------------------------------------
ParameterConnection::~ParameterConnection() {
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::onOperatorParameterChanged(void *src, const VstNumber &newValue) {
	// update a to refresh connection
	a->setValue(*a);
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::onChangedA(void *src, const VstNumber &newValue) {
	if (updateLock) // wichtig sonst: StackOverflow
		return;
	updateLock = true;
	VstNumber t = newValue;
	BOOST_FOREACH(ConnectionOperator::Ptr op, ops) {
		t = op->operate(t);
	}
	b->setValue(t);
	updateLock = false;
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::onChangedB(void *src, const VstNumber &newValue) {
	if (updateLock) // wichtig sonst: StackOverflow
		return;
	updateLock = true;
	VstNumber t = newValue;
	BOOST_FOREACH(ConnectionOperator::Ptr op, ops) {
		t = op->operateInverse(t);
	}
	a->setValue(t);
	updateLock = false;
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



