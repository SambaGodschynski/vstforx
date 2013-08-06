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
#include <sambag/com/BoostTimer2.hpp>

namespace processing {
namespace parameter {
namespace {
    const double MAX_INERTIA_DURATION = 1000.; // ms
    const double INERTIA_REFRESH_RATE = 30.; // ms
}
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
typedef Animation<double, DynamicTween, Updater, sambag::com::BoostTimer2> Tween;
Tween::Ptr getTween(boost::shared_ptr<void> t) {
    Tween::Ptr res;
    if (!t) {
        res = Tween::create();
        res->setRefreshRate(INERTIA_REFRESH_RATE);
        return res;
    } else {
        return boost::static_pointer_cast<Tween>(t);
    }
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
    inertiaDuration->setDisplay( "0 ms" );
    inertiaDuration->setLabel("ms");
    inertiaType = Parameter::create();
    inertiaType->setName("Inertia Type");
    inertiaType->setDisplay( "lin" );
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
	inertiaDuration->addTrackedValueChangedListener( 
		boost::bind(&ParameterConnection::onInertiaDurationChanged, this, _1, _2),
		self
	);
	inertiaType->addTrackedValueChangedListener(
		boost::bind(&ParameterConnection::onInertiaTypeChanged, this, _1, _2 ),
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
    Tween::Ptr tween;
    _tween = tween = getTween(_tween);
    if (tween->dst.lock() && tween->dst.lock() != p) {
        // tween is currently in use
        return;
    }
    if (!tween->dst.lock()) {
        tween->dst = p;
    }
    tween->setStartValue(p->getValue());
    tween->setEndValue(newValue);
    tween->setDuration(*inertiaDuration * MAX_INERTIA_DURATION);
    tween->start();
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::onChangedA(void *src, const VstNumber &newValue) {
	VstNumber t = newValue;
	BOOST_FOREACH(ConnectionOperator::Ptr op, ops) {
		t = op->operate(t);
	}
    if (*inertiaDuration!=0.) {
        update(b, t);
        return;
    }
    
    if (updateLock) {
        return;
    }
    updateLock = true;
    b->setValue(t);
    updateLock = false;
	
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::onChangedB(void *src, const VstNumber &newValue) {
	VstNumber t = newValue;
	BOOST_FOREACH(ConnectionOperator::Ptr op, ops) {
		t = op->operateInverse(t);
	}
    if (*inertiaDuration!=0.) {
        update(a, t);
        return;
    }
    
    if (updateLock) {
        return;
    }
    updateLock = true;
    a->setValue(t);
    updateLock = false;
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
    inertiaDuration->setDisplay( sambag::com::toString(value*MAX_INERTIA_DURATION) + " ms" );
}
//------------------------------------------------------------------------------------------------------------
void ParameterConnection::onInertiaTypeChanged(void *src, const float &value)
{
    Tween::Ptr tween;
    _tween = tween = getTween(_tween);
    if (!tween) {
        return;
    }
    int n = ::com::mapInteger(value, Tween::TweenPolicy::NUM_TYPES);
    tween->TweenPolicy::setTweenType( (Tween::TweenPolicy::Type)n );
    inertiaType->setDisplay( tween->TweenPolicy::toString() );
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



