/*
 * ===========================================================================================================
 * parameter.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include <limits> 
#include "parameter.h" 
#include <sambag/disco/components/Animation.hpp>
#include <sambag/disco/Tweens.hpp>
#include <sambag/com/Exception.hpp>

namespace processing {
namespace parameter {
//============================================================================================================
// Schnitstelle: ConnectionOperator.
//============================================================================================================
//============================================================================================================
// Klasse: ParameterConnection.
//============================================================================================================
using sambag::disco::components::Animation;
using sambag::disco::components::defaultTweens::DynamicTween;
template <class T>
struct Updater {
    Parameter::WPtr dst;
    void update(const T& val){
        Parameter::Ptr p = dst.lock();
        if (!p) {
            return;
        }
        p->setValue(val);
    }
    void finished(const T& val){
        dst = Parameter::Ptr();
    }
};
typedef Animation<double, DynamicTween, Updater> Tween;
Tween::Ptr getTweenPtr(boost::shared_ptr<void> t) {
    return boost::shared_static_cast<Tween>(t);
}
//------------------------------------------------------------------------------------------------------------
ParameterConnection::ParameterConnection(ParameterPtr a, ParameterPtr b) : 
	updateLock(false), a(a), b(b) 
{
    initInertiaParameter();
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::initInertiaParameter() {
    inertiaDuration = Parameter::create();
    inertiaDuration->setName("Inertia Duration");
    inertiaType = Parameter::create();
    inertiaType->setName("Inertia Type");
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
void ParameterConnection::update(ParameterPtr p, const VstNumber &newValue) {
  /*  Tween::Ptr tween;
    if (!_tween) {
        _tween = tween = Tween::create();
        tween->setRefreshRate(20.);
    } else {
        tween = getTweenPtr(_tween);
        SAMBAG_ASSERT(tween);
    }
    if (tween->dst.lock() && tween->dst.lock() != p) {
        return;
    }
    tween->stop();
    tween->setStartValue(p->getValue());
    tween->setEndValue(newValue);
    tween->setDuration(1000);
    if (!tween->dst.lock()) {
        tween->dst = p;
    }
    tween->start();
    //p->setValue(newValue);*/
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::onChangedA(void *src, const VstNumber &newValue) {
	VstNumber t = newValue;
	BOOST_FOREACH(ConnectionOperator::Ptr op, ops) {
		t = op->operate(t);
	}
	update(b, t);
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::onChangedB(void *src, const VstNumber &newValue) {
	VstNumber t = newValue;
	BOOST_FOREACH(ConnectionOperator::Ptr op, ops) {
		t = op->operateInverse(t);
	}
	update(a, t);
}
//------------------------------------------------------------------------------------------------------------
Parameter::Ptr ParameterConnection::getParameter ( size_t index ) const {
    switch(index) {
        case 0: return inertiaDuration;
        case 1: return inertiaType;
    }
    return Parameter::Ptr();
}
//------------------------------------------------------------------------------------------------------------
size_t ParameterConnection::getNumParameter () const {
    return 2;
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::onInertiaDurationChanged(void *src, const float &value)
{
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::onInertiaTypeChanged(void *src, const float &value) {
}
//============================================================================================================
// Klasse: Parameter.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
Parameter::Parameter( int index ):
updateLock(false), 
_min(0.0f),
_max(1.0f),
readOnly(false)
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
void Parameter::setReadOnly(bool val) {
	readOnly = val;
}

//------------------------------------------------------------------------------------------------------------
int Parameter::instances = 0;

} // namespace parameter
} // namespace processing



