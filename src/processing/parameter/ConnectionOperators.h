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

/**
 * TODO: new Op's have to be registered manually in ParameterConnection.cpp ConnectionOps AND
 * SerilizationRegister.
 * Consider a new approach such like ModelFactory with automatic registering
 */

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
		setName (name());
	}
public:
	//--------------------------------------------------------------------------------------------------------
	inline static std::string name() {
		return "Inverse Operator";
	};
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual com::VstNumber operate ( com::VstNumber dest ){
		return (com::VstNumber)(1.0 - dest);
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert inverseOperation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual com::VstNumber operateInverse ( com::VstNumber dest ){
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
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Offset-Parameterwert
	 */
	Parameter::Ptr offset;
	//--------------------------------------------------------------------------------------------------------
	OffsetConnection () : ConnectionOperator ()
	 {
		setName (name());
		offset = Parameter::create();
		offset->setName ("offset");
		*offset = 0.5f;
	}
public:
	//--------------------------------------------------------------------------------------------------------
	inline static std::string name() {
		return "Offset Operator";
	};
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
	virtual com::VstNumber operate ( com::VstNumber x ){
		return x + *offset - 0.5f;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert inverse Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual com::VstNumber operateInverse ( com::VstNumber x ){
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
	void initListener() {
		slope->addValueChangedListener(
			boost::bind(&ExpConnection::onSlopeChanged, this, _1, _2)
		);
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisiert ExpConnection-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void save ( com::oArchive &ar, const unsigned int version ) const {
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
	void load ( com::iArchive &ar, const unsigned int version ) {
		ar >> boost::serialization::base_object<ConnectionOperator> ( *this );
		ar >> a;
		ar >> slope;
		initListener();
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Slope-Paramter der Exp. funktion.
	 */
	Parameter::Ptr slope;
	//--------------------------------------------------------------------------------------------------------
	com::VstNumber a;
	//--------------------------------------------------------------------------------------------------------
	ExpConnection () : ConnectionOperator ( )
	 {
		setName (name());
		slope = Parameter::create();
		*slope = STD_SLOPE;
		slope->setMin(0.0001f);
		slope->setMax(0.9999f);
		a = -log ( *slope ); // == log (1/slope)
		initListener();
		ExpConnection::slope->setName ("EXP/LOG slope");
	}
	//--------------------------------------------------------------------------------------------------------
	void onSlopeChanged(void *src, const float &value) {
		a = -log( value );
	}
public:
	//--------------------------------------------------------------------------------------------------------
	inline static std::string name() {
		return "EXP Operator";
	};
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
	virtual com::VstNumber operate ( com::VstNumber x ){
		if ( x == 0.0f ) return 0.0001f; 
		return exp( a*x ) * *slope;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert inverse Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual com::VstNumber operateInverse ( com::VstNumber x ){
		if ( x == 0.0f ) return 0.0f; 
		float v = -log( x ) / -a  + 1;
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
// Proportionale Verbindung mit log Anstieg.
//============================================================================================================
class LogConnection : public ConnectionOperator, public HasParameter {
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<LogConnection> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	void initListener() {
		slope->addValueChangedListener(
			boost::bind(&LogConnection::onSlopeChanged, this, _1, _2)
		);
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisiert LogConnection-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void save ( com::oArchive &ar, const unsigned int version ) const {
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
	void load ( com::iArchive &ar, const unsigned int version ) {
		ar >> boost::serialization::base_object<ConnectionOperator> ( *this );
		ar >> a;
		ar >> slope;
		initListener();
	}
	//--------------------------------------------------------------------------------------------------------
	LogConnection () : ConnectionOperator () {	
		setName (name());
		slope = Parameter::create();
		*slope = STD_SLOPE;
		slope->setMin(0.0001f);
		slope->setMax(0.9999f);
		slope->setName("LOG/EXP slope");
		a = log( *slope );
		initListener();
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Slope-Parameter der Log. funktion
	 */
	Parameter::Ptr slope;
	//--------------------------------------------------------------------------------------------------------
	com::VstNumber a;
	//--------------------------------------------------------------------------------------------------------
	void onSlopeChanged(void *src, const float &value) {
		a = log( value );
	}
public:
	//--------------------------------------------------------------------------------------------------------
	inline static std::string name() {
		return "LOG Operator";
	};
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
	virtual com::VstNumber operate ( com::VstNumber x ){
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
	virtual com::VstNumber operateInverse ( com::VstNumber x ){
		if ( x == 0.0f ) return 0.0001f; 
		return exp( -a*x ) * *slope;
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
//============================================================================================================
// Klasse: MultiplierConnection.
// Proportionale Verbindung mit log Anstieg.
//============================================================================================================
class MultiplierConnection : public ConnectionOperator, public HasParameter {
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<MultiplierConnection> Ptr;
private:
    //--------------------------------------------------------------------------------------------------------
    static const float MaxFactor;
	//--------------------------------------------------------------------------------------------------------
	void initListener() {
        m->addValueChangedListener(
			boost::bind(&MultiplierConnection::onMulChanged, this, _1, _2)
		);
    }
    //--------------------------------------------------------------------------------------------------------
    void onMulChanged(void*, const float &value) {
        m->setDisplay(com::MyString(value*(float)MaxFactor));
    }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisiert LogConnection-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void save ( com::oArchive &ar, const unsigned int version ) const {
		ar << boost::serialization::base_object<ConnectionOperator> ( *this );
		ar << m;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Deserialisiert LogConnection-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void load ( com::iArchive &ar, const unsigned int version ) {
		ar >> boost::serialization::base_object<ConnectionOperator> ( *this );
		ar >> m;
		initListener();
	}
	//--------------------------------------------------------------------------------------------------------
	MultiplierConnection () : ConnectionOperator () {	
		setName (name());
		m = Parameter::create();
		*m = STD_SLOPE;
		m->setMin(0.0001f);
		m->setMax(1.0f);
		m->setName("multiplier");
		initListener();
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Slope-Parameter der Log. funktion
	 */
	Parameter::Ptr m;
public:
	//--------------------------------------------------------------------------------------------------------
	inline static std::string name() {
		return "Multiplier Operator";
	};
	//--------------------------------------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new MultiplierConnection());
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~MultiplierConnection(){}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual com::VstNumber operate ( com::VstNumber x ){
		return x * *m * MaxFactor;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert inverse Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual com::VstNumber operateInverse ( com::VstNumber x ) {
        if (*m==0) {
            SAMBAG_LOG_WARN<<"multiplier op div by zero";
            return FLT_MAX;
        }
		return x / (*m * MaxFactor);
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Slope-Parameter unabhaengig von index.
	 */
	virtual Parameter::Ptr getParameter ( size_t index ) const {
		return m;
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


