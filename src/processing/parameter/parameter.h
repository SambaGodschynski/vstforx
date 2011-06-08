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

namespace processing {
namespace parameter {
using namespace events;
using namespace std;
using namespace com;
//============================================================================================================
// Schnitstelle: ParameterListener.
//============================================================================================================
class ParameterListener;
//============================================================================================================
// Schnitstelle: HasParameter.
//============================================================================================================
class HasParameter;
//============================================================================================================
// Klasse: Parameter.
// Repraesentiert alle PPI VST-Parameter die als schnitstelle zum
// Host dienen.
//============================================================================================================
class Parameter;
//============================================================================================================
// Schnittstelle: ConnectionOperator.
//============================================================================================================
class ConnectionOperator;
//============================================================================================================
// Vorwaerts Deklarierte Shared Ptr.
//============================================================================================================
typedef boost::shared_ptr<Parameter> ParameterPtr;
} // namespace parameter
} // namespace processing


namespace processing {
namespace parameter {
//============================================================================================================
// Schnitstelle: HasParameter.
//============================================================================================================
class HasParameter{
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<HasParameter> Ptr;
	//--------------------------------------------------------------------------------------------------------
	virtual ParameterPtr getParameter ( size_t nr = 0 ) const = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const = 0;
};
//============================================================================================================
// Schnitstelle: ConnectionOperator.
//============================================================================================================
class ConnectionOperator {
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<ConnectionOperator> Ptr;
	//--------------------------------------------------------------------------------------------------------
	typedef list<ConnectionOperator::Ptr> Container; 
private:
	//--------------------------------------------------------------------------------------------------------
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
	Parameter *u,*v; // nicht shared_ptr! sonst haelt op. param. der op haelt => leak 
public:
	//--------------------------------------------------------------------------------------------------------
	const string getName() const { return name; }
	//--------------------------------------------------------------------------------------------------------
	void setName ( const string &_name ) { name = _name; }
	//--------------------------------------------------------------------------------------------------------
	Parameter * getParameterA() const { return u; }
	//--------------------------------------------------------------------------------------------------------
	Parameter * getParameterB() const { return v; }
	//--------------------------------------------------------------------------------------------------------
	virtual float operate ( float f ) = 0;
	//--------------------------------------------------------------------------------------------------------
	// Liefert ConnectionOperator mit Umkehrfunktion. 
	virtual ConnectionOperator * newInvereseOperator() = 0;
	//--------------------------------------------------------------------------------------------------------
	ConnectionOperator( Parameter *u, Parameter *v );
	//--------------------------------------------------------------------------------------------------------
	virtual ~ConnectionOperator() {}
};
//============================================================================================================
// Klasse: Parameter.
// Repraesentiert alle PPI VST-Parameter die als schnitstelle zum
// Host dienen.
//============================================================================================================
class Parameter : 
	public ValueChangedSender<float>, 
	public PObject,
	public Serializable
{
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
	typedef map <U, V> ParameterConnection;
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
	//Stuff text with the name 
    //("Time", "Gain", "RoomType", etc...) of parameter index.
    MyString name;
	//--------------------------------------------------------------------------------------------------------
	//Stuff text with a string representation 
    //("0.5", "-3", "PLATE", etc...) of the value of parameter index.
    MyString display;
	//--------------------------------------------------------------------------------------------------------
	//Stuff label with the units in which parameter index is displayed 
    //(i.e. "sec", "dB", "type", etc...). 
    MyString label;
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version );
protected:
	//--------------------------------------------------------------------------------------------------------
	// updateConnection Sperre:
	// ist updateLock true werden setValue() aufrufe ignoriert.
	// Dies verhindert evntl.indirekte Rekursion beim 
	// aufruf von updateConnection die entshehen wenn 
	// Paramter Connection zyklisch sind.
	bool updateLock;
	//--------------------------------------------------------------------------------------------------------
	Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	// Parameter Connections
	ParameterConnection connections;
	//--------------------------------------------------------------------------------------------------------
	// durchlaeuft alle Parameter in connections, setzt dort eigenen
	// Parameter wert und ruft operate() methode
	// der Assoziierten ConnectionOperator Objekte auf.
	virtual void updateConnections();
	//--------------------------------------------------------------------------------------------------------
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
	virtual void _removeConnection ( Parameter *dst );
	//--------------------------------------------------------------------------------------------------------
	Parameter( int index = 0 );
public:
	//--------------------------------------------------------------------------------------------------------
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
	int getParameterNr() { return nr; }
	//--------------------------------------------------------------------------------------------------------
	int getIndex() const { return index; }
	//--------------------------------------------------------------------------------------------------------
	void setIndex( int i ) { index = i; }
	//--------------------------------------------------------------------------------------------------------
	const MyString & getName() const { return name; }
	//--------------------------------------------------------------------------------------------------------
	void setName(const MyString &name){ Parameter::name = name.trim(); }
	//--------------------------------------------------------------------------------------------------------
	const MyString & getLabel() const { return label; }
	//--------------------------------------------------------------------------------------------------------
	void setLabel(const MyString &label){ Parameter::label = label; }
	//--------------------------------------------------------------------------------------------------------
	virtual VstNumber getValue() const { 
		return value;
	}
	//--------------------------------------------------------------------------------------------------------
	// setzt den Parameterwert auf v.
	// Benachrichtigt alle verbundenen ProcessorNodes und Listener.
	virtual void setValue( VstNumber v ){
		if ( updateLock ) return;
		value = com::getMin<VstNumber>( _max, com::getMax<VstNumber>( _min, v ) );
		notifyListeners(this, *this);
		updateConnections();
	}
	//--------------------------------------------------------------------------------------------------------
	// setzt den Parameterwert auf v.
	// Benachrichtigt alle verbundenen ProcessorNodes und Listener.
	// uberspringt benachrichtigung von skipThis. ( z.b um feedback callbacks zu vermeiden )
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
	MyString  getDisplay() const {
		if (!display.empty()){
			return display;
		}
		return MyString( getValue() );
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void setMin ( VstNumber v ){ _min = v; }
	//--------------------------------------------------------------------------------------------------------
	virtual void setMax ( VstNumber v ){ _max = v; }
	//--------------------------------------------------------------------------------------------------------
	virtual VstNumber getMin() { return _min; }
	//--------------------------------------------------------------------------------------------------------
	virtual VstNumber getMax() { return _max; }
	//--------------------------------------------------------------------------------------------------------
	void setDisplay(const MyString &display){ Parameter::display = display; }
	//--------------------------------------------------------------------------------------------------------
	virtual bool addBiConnection ( Parameter *dest ){
		if ( dest == this ) return false;
		return _addConnection ( dest ) && dest->_addConnection ( this );
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void addConnectionOperator ( Parameter *dst, const ConnectionOperator::Ptr &pProcessor ){
		TRY_TO_LOCK_TIMED(mutex);
		ParameterConnection::iterator it = connections.find ( dst );
		if ( it == connections.end() ) 
			throw ppiError::IndexOutOfBoundException("No Connection", __FILE__, __LINE__ );
		(*it).second.push_back ( ConnectionOperator::Ptr(pProcessor) );
	}
	//--------------------------------------------------------------------------------------------------------
	virtual const ConnectionOperator::Container & getConnectionOperatorContainer ( Parameter *dst ) const {
		ParameterConnection::const_iterator it = connections.find ( dst );
		if ( it == connections.end() ) 
			throw ppiError::IndexOutOfBoundException("No Connection", __FILE__, __LINE__ );
		return (*it).second;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void removeConnectionOperator ( Parameter *src, const ConnectionOperator::Ptr &pProcessor ){
		TRY_TO_LOCK_TIMED(mutex);
		ParameterConnection::iterator it = connections.find ( src );
		if ( it == connections.end() ) return;
		(*it).second.remove ( pProcessor );
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void removeBiConnection ( Parameter *dst );
	//--------------------------------------------------------------------------------------------------------
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



