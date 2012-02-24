/*
 * ===========================================================================================================
 * ConnectionOperators.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef CONNECTION_OPS
#define CONNECTION_OPS

#include "com/one4All.h"
#include "parameter.h"
#include "com/Serialization.h"
#include <math.h>


namespace processing{
namespace parameter {
//============================================================================================================
// Vorwaerz deklarationen
//============================================================================================================
class InverseConnection;
class OffsetConnection;
class ExpConnection;
class LogConnection;
class RelativeConnection;

} //namespace parameter 
} //namespace processing


static const float STD_SLOPE = 0.01f;

namespace processing{
namespace parameter {
//============================================================================================================
/**
 * Klasse: InverseConnection.
 * Umgekehrt Proportionale Verbindung.
 */
//============================================================================================================
class InverseConnection : public ConnectionOperator {
friend class boost::serialization::access;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisierung eines InverseConnection-Objektes
	 * @param ar
	 * @param version
	 */
	template <typename Archiv>
	void serialize ( Archiv &ar, const unsigned int version ){
		ar & boost::serialization::base_object<ConnectionOperator> ( *this );
	}
	//--------------------------------------------------------------------------------------------------------
	InverseConnection (){}
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual float operate ( float dest ){
		return 1.0 - dest;
	}
	//--------------------------------------------------------------------------------------------------------
	InverseConnection ( Parameter *u, Parameter *v ) : ConnectionOperator ( u, v ) {
		setName ("Inverse Operator");
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return InverseConnection-Objekt
	 */
	virtual ConnectionOperator * newInvereseOperator() { return new InverseConnection(u,v); }
};
//============================================================================================================
/**
 * @class OffsetConnection.
 * fuegt Verbindung Verschiebungswert hinzu.
 */
class OffsetConnection : public ConnectionOperator, public HasParameter {
//============================================================================================================
friend class boost::serialization::access;
private:
	//--------------------------------------------------------------------------------------------------------
	bool inverse; // is inverse?
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert OffsetConnection-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
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
	/**
	 * Offset-Parameterwert
	 */
	Parameter::Ptr offset;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Offset-Parameterwert Handler
	 * @param src
	 * @param p
	 */
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
	/**
	 * implementiert Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual float operate ( float x ){
		return inverse ?  x - *offset + 0.5f : x + *offset - 0.5f;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Offset-Parameter, unabhaengig von index
	 */
	virtual Parameter::Ptr getParameter ( size_t index ) const {
		return offset;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return 1
	 */
	virtual size_t getNumParameter () const { return 1; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @return OffsetConnection-Objekt, wobei inverse = true
	 */
	virtual ConnectionOperator * newInvereseOperator();
};

//============================================================================================================
/**
 * @class ExpConnection.
 * Verbindung mit exponentialen Anstieg.
 */
class ExpConnection : public ConnectionOperator, public HasParameter {
//============================================================================================================
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisiert ExpConnection-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void save ( oArchive &ar, const unsigned int version ) const {
		ar << boost::serialization::base_object<ConnectionOperator> ( *this );
		ar << a;
		ar << slope;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert ExpConnection-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void load ( iArchive &ar, const unsigned int version ) {
		ar >> boost::serialization::base_object<ConnectionOperator> ( *this );
		ar >> a;
		ar >> slope;
		Parameter::ParameterListenerFunction f = 
			boost::bind( &ExpConnection::parameterChanged, this, _1, _2 );
		slope->addValueChangedListenerF (f);
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Slope-Paramter der Exp. funktion.
	 */
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
	/**
	 * implementiert Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual float operate ( float x ){
		if ( x == 0.0f ) return 0.0001f; 
		return exp( a*x ) * *slope;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Slope-Parameter, unabhaengig von index
	 */
	virtual Parameter::Ptr getParameter ( size_t index ) const {
		return slope;
	}
	/**
	 * @return 1
	 */
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const { return 1; }

	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return LogConnection-Objekt
	 */
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
	/**
	 * Serialisiert LogConnection-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void save ( oArchive &ar, const unsigned int version ) const {
		ar << boost::serialization::base_object<ConnectionOperator> ( *this );
		ar << a;
		ar << slope;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Deserialisiert LogConnection-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
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
	/**
	 * Slope-Parameter der Log. funktion
	 */
	Parameter::Ptr slope;
	//--------------------------------------------------------------------------------------------------------
	VstNumber a;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Slope-Paramter Handler
	 * @param src
	 * @param p
	 */
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
	/**
	 * implementiert Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual float operate ( float x ){
		if ( x == 0.0f ) return 0.0f; 
		float v = -log( x ) / a  + 1;
		// problem with ambience reverb when using log/exp to dry/wet.
		// soundoutput will stop. following limitation will handle it:		
		if (v<0.0f)
			return 0.0f;
		if (v>1.0f)
			return 1.0f;
		return v;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Slope-Parameter unabhaengig von index.
	 */
	virtual Parameter::Ptr getParameter ( size_t index ) const {
		return slope;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return 1
	 */
	virtual size_t getNumParameter () const { return 1; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @return ExpConnection-Objekt
	 */
	virtual ConnectionOperator * newInvereseOperator();
};
} //namespace parameter 
} //namespace processing


#endif


