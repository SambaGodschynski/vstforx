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
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<InverseConnection> Ptr;
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
	InverseConnection () : ConnectionOperator () {
		setName ("Inverse Operator");
	}
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual VstNumber operate ( VstNumber dest ){
		return 1.0 - dest;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert inverseOperation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual VstNumber operateInverse ( VstNumber dest ){
		return operate(dest);
	}
	//--------------------------------------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new InverseConnection());
	}
};
//============================================================================================================
/**
 * @class OffsetConnection.
 * fuegt Verbindung Verschiebungswert hinzu.
 */
class OffsetConnection : public ConnectionOperator, public HasParameter {
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<OffsetConnection> Ptr;
private:
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
	OffsetConnection () : ConnectionOperator ()
	 {
		setName ("Offset Operator");
		offset = Parameter::create();
		offset->setName ("offset");
		Parameter::ParameterListenerFunction f = 
			boost::bind( &OffsetConnection::parameterChanged, this, _1, _2 );
		offset->addValueChangedListenerF (f);
		*offset = 0.5f;
	}
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new OffsetConnection());
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~OffsetConnection() {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual VstNumber operate ( VstNumber x ){
		return x + *offset - 0.5f;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert inverse Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual VstNumber operateInverse ( VstNumber x ){
		return x - *offset + 0.5f;
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
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<ExpConnection> Ptr;
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
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Slope-Paramter der Exp. funktion.
	 */
	Parameter::Ptr slope;
	//--------------------------------------------------------------------------------------------------------
	VstNumber a;
	//--------------------------------------------------------------------------------------------------------
	ExpConnection () : ConnectionOperator ( )
	 {
		setName ("EXP Operator");
		slope = Parameter::create();
		*slope = STD_SLOPE;
		slope->setMin(0.0001f);
		slope->setMax(0.9999f);
		a = -log ( *slope ); // == log (1/slope)
		ExpConnection::slope->setName ("EXP/LOG slope");
	}
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new ExpConnection());
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~ExpConnection (){}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual VstNumber operate ( VstNumber x ){
		if ( x == 0.0f ) return 0.0001f; 
		return exp( a*x ) * *slope;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert inverse Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual VstNumber operateInverse ( VstNumber x ){
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
};
//============================================================================================================
// Klasse: LogConnection.
// Proportionale Verbindung mit Log anstieg.
//============================================================================================================
class LogConnection : public ConnectionOperator, public HasParameter {
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<LogConnection> Ptr;
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
	}
	//--------------------------------------------------------------------------------------------------------
	LogConnection () : ConnectionOperator () {	
		setName ("LOG Operator");
		slope = Parameter::create();
		*slope = STD_SLOPE;
		slope->setMin(0.0001f);
		slope->setMax(0.9999f);
		slope->setName("LOG/EXP slope");
		a = log( *slope );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Slope-Parameter der Log. funktion
	 */
	Parameter::Ptr slope;
	//--------------------------------------------------------------------------------------------------------
	VstNumber a;
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new LogConnection());
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~LogConnection(){}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual VstNumber operate ( VstNumber x ){
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
	 * implementiert inverse Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual VstNumber operateInverse ( VstNumber x ){
		if ( x == 0.0f ) return 0.0001f; 
		return exp( a*x ) * *slope;
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
};
} //namespace parameter 
} //namespace processing


#endif


