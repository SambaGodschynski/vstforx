/*
 * ===========================================================================================================
 * parameter.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef PPICORE_PPIAPP_H
#define PPICORE_PPIAPP_H

#include <vector>
#include <string>
#include <set>
#include <map>
#include "com/one4All.h"
#include "com/Events.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "processing/PObject.h"
#include <boost/math/special_functions/fpclassify.hpp>

namespace processing {
namespace parameter {
using namespace events;
using namespace std;
using namespace com;
//============================================================================================================
// Vorwaertz deklarationen
//============================================================================================================
class ParameterListener;
class HasParameter;
class Parameter;
class ConnectionOperator;
typedef boost::shared_ptr<Parameter> ParameterPtr;
} // namespace parameter
} // namespace processing


namespace processing {
namespace parameter {
//============================================================================================================
/**
 *  @interface: HasParameter.
 */
class HasParameter{
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<HasParameter> Ptr;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Parameter zu index.
	 */
	virtual ParameterPtr getParameter ( size_t nr = 0 ) const = 0;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller Parameter.
	 */
	virtual size_t getNumParameter () const = 0;
};
//============================================================================================================
/**
 * @class ConnectionOperator.
 * Oberklasse fuer Parameter-Verbindungs-Operator.
 */
class ConnectionOperator {
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<ConnectionOperator> Ptr;
	//--------------------------------------------------------------------------------------------------------
	typedef list<ConnectionOperator::Ptr> Container; 
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (DE)Serialisierung eines ConnectionOperator-Objektes.
	 * @param ar boost::Archive-Objekt.
	 * @param version
	 */
	template < typename Archive >
	void serialize( Archive &ar, const unsigned int version ){
		ar & u;
		ar & v;
	}
	//--------------------------------------------------------------------------------------------------------
	string name;
protected:
	//--------------------------------------------------------------------------------------------------------
	ConnectionOperator(){}
	//--------------------------------------------------------------------------------------------------------
	Parameter *u,*v;// TODO: existeren nur zur OperatorParamter(zb.:slope)
					// nach ConnectionParameter kommunikation / alternative finden
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Operatorname
	 */
	const string getName() const { return name; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Operatorname
	 * @param _name
	 */
	void setName ( const string &_name ) { name = _name; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parameter A der Verbindung
	 */
	Parameter * getParameterA() const { return u; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parameter B der Verbindung
	 */
	Parameter * getParameterB() const { return v; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual float operate ( float f ) = 0;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return inverser ConnectionOperator
	 */
	virtual ConnectionOperator * newInvereseOperator() = 0;
	//--------------------------------------------------------------------------------------------------------
	ConnectionOperator( Parameter *u, Parameter *v );
	//--------------------------------------------------------------------------------------------------------
	virtual ~ConnectionOperator() {}
};
//============================================================================================================
/**
 * @class Parameter.
 * Kann Fliesskommawert anehmen.
 * Parameter sind untereinander verbindbar.Parameterverbindungen koennen
 * mit Operatoren versehen werden. Diese beeinflussen die
 * Verbindungswert uebertragung.
 */
class Parameter : 
	public ValueChangedSender<float>, 
	public PObject,
	public Serializable
{
//============================================================================================================
friend class boost::serialization::access; 
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<Parameter> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	typedef Parameter* U; //verbundener Parameter
	//--------------------------------------------------------------------------------------------------------
	typedef ConnectionOperator::Container V; //Operatoren
	//--------------------------------------------------------------------------------------------------------
	typedef map <U, V> ParameterConnection; // TODO: Verbindung als Klasse
	//--------------------------------------------------------------------------------------------------------
	VstNumber _min, _max;
	//--------------------------------------------------------------------------------------------------------
	// Wert des Parameters;
	VstNumber value;
	//--------------------------------------------------------------------------------------------------------
	int nr;
	//--------------------------------------------------------------------------------------------------------
	// fuer VST-Plugin Parameter kommunikation
	int index;
	//--------------------------------------------------------------------------------------------------------
	static int instances;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * nach VST-SDK:
	 * Stuff text with the name
	 * ("Time", "Gain", "RoomType", etc...) of parameter index.
	 */
	MyString name;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * nach VST-SDK:
	 * Stuff text with a string representation
	 * ("0.5", "-3", "PLATE", etc...) of the value of parameter index.
	 */
	MyString display;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * nach VST-SDK:
	 * Stuff label with the units in which parameter index is displayed
	 * (i.e. "sec", "dB", "type", etc...).
	 */
	MyString label;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisierung von Parameter-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version );
protected:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * updateConnection Sperre:
	 * verhindert evntl.indirekte Rekursion wenn
	 * Paramter Connection zyklisch sind.
	 */
	bool updateLock;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * blockt updateConnections() gegen removeConnection()
	 */
	Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	// Parameter Connections
	ParameterConnection connections;
	//--------------------------------------------------------------------------------------------------------
	/**
	 *  aktualisiert Parameterverbindungen.
	 */
	virtual void updateConnections();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * fuegt gerichtete Parameterverbindung hinzu.
	 * @param dest
	 * @return true, wenn erfolgt
	 */
	virtual bool _addConnection ( Parameter *dest ){
		TRY_TO_LOCK_TIMED(mutex);
		if (!dest) return false;
		pair < ParameterConnection::iterator, bool > ret = connections.insert ( 
			pair< Parameter*, ConnectionOperator::Container > ( dest, 
																	list< ConnectionOperator::Ptr >() 
															       ) 
		);
		return ret.second;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * entfernt gerichtete Parameterverbindung.
	 * @param dst
	 */
	virtual void _removeConnection ( Parameter *dst );
	//--------------------------------------------------------------------------------------------------------
	Parameter( int index = 0 );
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index optionaler Index, kann zur idendifizierung benutzt werden.
	 * @return neues Parameter-Objekt
	 */
	static Ptr create ( int index = 0 ) {
		Ptr neu ( new Parameter(index) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	typedef ValueChangedSender<VstNumber>::ValueChangedFunction ParameterListenerFunction;
	//--------------------------------------------------------------------------------------------------------
	virtual ~Parameter();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @deprecated TODO: entfernen
	 * globaler Index.
	 * @return
	 */
	int getParameterNr() { return nr; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Index
	 */
	int getIndex() const { return index; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Index. Kann zur idendifizierung benutzt werden.
	 * @param i
	 */
	void setIndex( int i ) { index = i; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parametername
	 */
	const MyString & getName() const { return name; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Parametername
	 * @param name
	 */
	void setName(const MyString &name){ Parameter::name = name.trim(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parameterwert als String  ("0.5", "-3", "PLATE", etc...)
	 */

	const MyString & getLabel() const { return label; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt String-Parameterwert
	 * @param label
	 */
	void setLabel(const MyString &label){ Parameter::label = label; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parameterwert
	 */
	virtual VstNumber getValue() const { 
		return value;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Parameterwert.
	 * @param v
	 */
	virtual void setValue( VstNumber v ){
		if ( updateLock ) return;
		// avoid NaN. problems with serialize and deserialize
		// see: issue #113
		if ( !boost::math::isfinite(v) ) { 
			v = 0;
		}
		value = com::getMin<VstNumber>( _max, com::getMax<VstNumber>( _min, v ) );
		notifyListeners(this, *this);
		updateConnections();
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @deprecated TODO: entfernen
	 * @param setzt Parameterwert.
	 * @param skipThis ueberspringt Listener
	 */
	virtual void setValue( VstNumber v, ParameterListenerFunction &skipThis ){
		if ( updateLock ) return;
		value = com::getMin<VstNumber>( _max, com::getMax<VstNumber>( _min, v ) );
		notifyListeners( this, *this, skipThis );
		updateConnections();
	}
	//--------------------------------------------------------------------------------------------------------
	void operator=(VstNumber v){ setValue (v); }
	//--------------------------------------------------------------------------------------------------------
	operator VstNumber() { return getValue(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parametereinheit (nach VST-SDK i.e. "sec", "dB", "type", etc...).
	 */
	MyString  getDisplay() const {
		if (!display.empty()){
			return display;
		}
		return MyString( getValue() );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Parametereinheit (nach VST-SDK i.e. "sec", "dB", "type", etc...).
	 * @param display
	 */
	void setDisplay(const MyString &display){ Parameter::display = display; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Minimalwert den Parameter annehmen kann
	 * @param v
	 */
	virtual void setMin ( VstNumber v ){ _min = v; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Maximalwert den Parameter annehmen kann
	 * @param v
	 */
	virtual void setMax ( VstNumber v ){ _max = v; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parameter-Minimum
	 */
	virtual VstNumber getMin() { return _min; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parameter-Maximum
	 */
	virtual VstNumber getMax() { return _max; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * fuegt bidirektionale Verbindung zwischen hinzu (this<->dest).
	 * Keine Mehrfachverbindungen.
	 * @param dest
	 * @return true, wenn erfolgt
	 */
	virtual bool addBiConnection ( Parameter *dest ){
		if ( dest == this ) return false;
		return _addConnection ( dest ) && dest->_addConnection ( this );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * fuegt bidirektionaler Verbindung (this<->dst) Operatoren hinzu:
	 * this->dst: Operator
	 * dst->this: Operator->getInverseOperator()
	 * @param dst
	 * @param pProcessor
	 */
	virtual void addConnectionOperator ( Parameter *dst, const ConnectionOperator::Ptr &op ){
		TRY_TO_LOCK_TIMED(mutex);
		ParameterConnection::iterator it = connections.find ( dst );
		if ( it == connections.end() ) 
			throw ppiError::IndexOutOfBoundException("No Connection", __FILE__, __LINE__ );
		(*it).second.push_back ( ConnectionOperator::Ptr(op) );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param dst
	 * @return Operatoren zur bidirektionalen Verbindung this<->dst.
	 */
	virtual const ConnectionOperator::Container & getConnectionOperators ( Parameter *dst ) const {
		ParameterConnection::const_iterator it = connections.find ( dst );
		if ( it == connections.end() ) 
			throw ppiError::IndexOutOfBoundException("No Connection", __FILE__, __LINE__ );
		return (*it).second;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * entfernt Operatoren zur bidirektionalen Verbindung this<->dst.
	 * @param src
	 * @param pProcessor
	 */
	virtual void removeConnectionOperator ( Parameter *src, const ConnectionOperator::Ptr &pProcessor ){
		TRY_TO_LOCK_TIMED(mutex);
		ParameterConnection::iterator it = connections.find ( src );
		if ( it == connections.end() ) return;
		(*it).second.remove ( pProcessor );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * entfernt bidirektionale Verbindung this<->dst.
	 * @param dst
	 */
	virtual void removeBiConnection ( Parameter *dst );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller Parameter-Verbindungen
	 */
	virtual int getNumConnections () { return connections.size(); }
};
//============================================================================================================
// Klasse: Parameter.
// Template defs:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archiv >
void Parameter::serialize( Archiv &ar, const unsigned int version) {
	ar & boost::serialization::base_object<PObject>(*this);
	ar & name;
	ar & label;
	ar & display;
	ar & _min;
	ar & _max;
	ar & value;
	ar & nr;
	ar & index;
	ar & connections;
}
} // namespace parameter
} // namespace processing
#endif



