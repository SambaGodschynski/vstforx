#ifndef CONNECTION_OPS
#define CONNECTION_OPS

#include "com/one4All.h"
#include "parameter.h"
#include "com/Serialization.h"
#include <math.h>


namespace processing{
namespace parameter {
//============================================================================================================
// Klasse: InverseConnection.
// Umgekehrt Proportionale Verbindung.
// 1.0 -> 0.0 | ... -> ... | 0.5 -> 0.5 | ...->... | 0.0 -> 1.0
//============================================================================================================
class InverseConnection;
//============================================================================================================
// Klasse: OffsetConnection.
// adds an offset: -0.5 0 +0.5
//============================================================================================================
class OffsetConnection;
//============================================================================================================
// Klasse: ExpConnection.
// Proportionale Verbindung mit Exponentialen anstieg.
//============================================================================================================
class ExpConnection;
//============================================================================================================
// Klasse: ExpConnection.
// Proportionale Verbindung mit Log. anstieg.
//============================================================================================================
class LogConnection;
//============================================================================================================
// Klasse: RelativeConnection.
//============================================================================================================
class RelativeConnection;

} //namespace parameter 
} //namespace processing

#define STD_SLOPE 0.01f

namespace processing{
namespace parameter {
//============================================================================================================
// Klasse: InverseConnection.
// Umgekehrt Proportionale Verbindung.
// 1.0 -> 0.0 | ... -> ... | 0.5 -> 0.5 | ...->... | 0.0 -> 1.0
//============================================================================================================
class InverseConnection : public ConnectionOperator {
friend class boost::serialization::access;
private:
	//--------------------------------------------------------------------------------------------------------
	template <typename Archiv>
	void serialize ( Archiv &ar, const unsigned int version ){
		ar & boost::serialization::base_object<ConnectionOperator> ( *this );
	}
	//--------------------------------------------------------------------------------------------------------
	InverseConnection (){}
public:
	//--------------------------------------------------------------------------------------------------------
	virtual float operate ( float dest ){
		return 1.0 - dest;
	}
	//--------------------------------------------------------------------------------------------------------
	InverseConnection ( Parameter *u, Parameter *v ) : ConnectionOperator ( u, v ) {
		setName ("Inverse Operator");
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ConnectionOperator * newInvereseOperator() { return new InverseConnection(u,v); }
};
//============================================================================================================
// Klasse: OffsetConnection.
// adds an offset: -0.5 0 +0.5
//============================================================================================================
class OffsetConnection : public ConnectionOperator, public HasParameter {
friend class boost::serialization::access;
private:
	//--------------------------------------------------------------------------------------------------------
	bool inverse; // is inverse?
	//--------------------------------------------------------------------------------------------------------
	template <typename Archive>
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & boost::serialization::base_object<ConnectionOperator> ( *this );
		ar & offset;
		ar & inverse;
		if ( Archive::is_loading::value ) {
			Parameter::ParameterListenerFunction f = 
			boost::bind( &OffsetConnection::parameterChanged, this, _1, _2 );
			offset->addValueChangedListenerF (f);
		}
	}
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr offset;
	//--------------------------------------------------------------------------------------------------------
	void parameterChanged ( void *src, const float &p );
	//--------------------------------------------------------------------------------------------------------
	OffsetConnection (){}
public:
	//--------------------------------------------------------------------------------------------------------
	OffsetConnection ( Parameter *u, Parameter *v ) : ConnectionOperator ( u, v ), inverse(false)
	 {
		setName ("Offset Operator");
		offset = Parameter::create();
		offset->setName ("offset");
		Parameter::ParameterListenerFunction f = 
			boost::bind( &OffsetConnection::parameterChanged, this, _1, _2 );
		offset->addValueChangedListenerF (f);
		*offset = 0.5f;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~OffsetConnection() {}
	//--------------------------------------------------------------------------------------------------------
	virtual float operate ( float x ){
		return inverse ?  x - *offset + 0.5f : x + *offset - 0.5f;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual Parameter::Ptr getParameter ( size_t index ) const {
		return offset;
	}

	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const { return 1; }

	//--------------------------------------------------------------------------------------------------------
	virtual ConnectionOperator * newInvereseOperator();
};

//============================================================================================================
// Klasse: ExpConnection.
// Proportionale Verbindung mit Exponentialen anstieg.
//============================================================================================================
class ExpConnection : public ConnectionOperator, public HasParameter {
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
private:
	//--------------------------------------------------------------------------------------------------------
	void save ( oArchive &ar, const unsigned int version ) const {
		ar << boost::serialization::base_object<ConnectionOperator> ( *this );
		ar << a;
		ar << slope;
	}
	//--------------------------------------------------------------------------------------------------------
	void load ( iArchive &ar, const unsigned int version ) {
		ar >> boost::serialization::base_object<ConnectionOperator> ( *this );
		ar >> a;
		ar >> slope;
		Parameter::ParameterListenerFunction f = 
			boost::bind( &ExpConnection::parameterChanged, this, _1, _2 );
		slope->addValueChangedListenerF (f);
	}
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr slope;
	//--------------------------------------------------------------------------------------------------------
	VstNumber a;
	//--------------------------------------------------------------------------------------------------------
	void parameterChanged ( void *src, const float &p ){ 
		a = -log (p); 
		u->setValue(u->getValue()); 
	} 
	//--------------------------------------------------------------------------------------------------------
	ExpConnection (){}
public:
	//--------------------------------------------------------------------------------------------------------
	ExpConnection ( Parameter *u, Parameter *v ) : ConnectionOperator ( u, v )
	 {
		setName ("EXP Operator");
		slope = Parameter::create();
		*slope = STD_SLOPE;
		slope->setMin(0.0001f);
		slope->setMax(0.9999f);
		a = -log ( *slope ); // == log (1/slope)
		ExpConnection::slope->setName ("EXP/LOG slope");
		Parameter::ParameterListenerFunction f = 
			boost::bind( &ExpConnection::parameterChanged, this, _1, _2 );
		ExpConnection::slope->addValueChangedListenerF (f);
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~ExpConnection (){}
	//--------------------------------------------------------------------------------------------------------
	virtual float operate ( float x ){
		if ( x == 0.0f ) return 0.0001f; 
		return exp( a*x ) * *slope;
	}

	//--------------------------------------------------------------------------------------------------------
	virtual Parameter::Ptr getParameter ( size_t index ) const {
		return slope;
	}

	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const { return 1; }

	//--------------------------------------------------------------------------------------------------------
	virtual ConnectionOperator * newInvereseOperator();
};
//============================================================================================================
// Klasse: LogConnection.
// Proportionale Verbindung mit Log anstieg.
//============================================================================================================
class LogConnection : public ConnectionOperator, public HasParameter {
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
private:
	//--------------------------------------------------------------------------------------------------------
	void save ( oArchive &ar, const unsigned int version ) const {
		ar << boost::serialization::base_object<ConnectionOperator> ( *this );
		ar << a;
		ar << slope;
	}
	//--------------------------------------------------------------------------------------------------------
	void load ( iArchive &ar, const unsigned int version ) {
		ar >> boost::serialization::base_object<ConnectionOperator> ( *this );
		ar >> a;
		ar >> slope;
		Parameter::ParameterListenerFunction f = 
			boost::bind( &LogConnection::parameterChanged, this, _1, _2 );
		slope->addValueChangedListenerF (f);
	}
	//--------------------------------------------------------------------------------------------------------
	LogConnection (){}
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr slope;
	//--------------------------------------------------------------------------------------------------------
	VstNumber a;
	//--------------------------------------------------------------------------------------------------------
	void parameterChanged ( void *src, const float &p ) { 
		a = log ( p ); 
		u->setValue(u->getValue()); 
	} 
public:
	//--------------------------------------------------------------------------------------------------------
	LogConnection ( Parameter *u, Parameter *v ) : ConnectionOperator ( u, v ) {	
		setName ("LOG Operator");
		slope = Parameter::create();
		*slope = STD_SLOPE;
		slope->setMin(0.0001f);
		slope->setMax(0.9999f);
		slope->setName("LOG/EXP slope");
		a = log( *slope );
		ValueChangedSender<float>::ValueChangedFunction f = 
			boost::bind( &LogConnection::parameterChanged, this, _1, _2 );
		LogConnection::slope->addValueChangedListenerF (f);
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~LogConnection(){}
	//--------------------------------------------------------------------------------------------------------
	virtual float operate ( float x ){
		if ( x == 0.0f ) return 0.0f; 
		float v = -log( x ) / a  + 1;
		// problem with ambience reverb when using log/exp to dry/wet.
		// soundoutput will stop. following limitation will handle it:		
		/*if (v<0.0f) 
			return 0.0f;
		if (v>1.0f)
			return 1.0f;*/
		return v;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual Parameter::Ptr getParameter ( size_t index ) const {
		return slope;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const { return 1; }
	//--------------------------------------------------------------------------------------------------------
	virtual ConnectionOperator * newInvereseOperator();
};
} //namespace parameter 
} //namespace processing


#endif


