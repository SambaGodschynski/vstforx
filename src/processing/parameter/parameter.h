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
#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>


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
	}
	//--------------------------------------------------------------------------------------------------------
	string name;
protected:
	//--------------------------------------------------------------------------------------------------------
	ConnectionOperator(){}
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
	 * implementiert Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual VstNumber operate ( VstNumber f ) = 0;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert inverse Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual VstNumber operateInverse ( VstNumber f ) = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual ~ConnectionOperator() {}
};
//============================================================================================================
/**
 * @class ParameterConnection.
 * Repraesentiert Parameter-Verbindung.
 */
class ParameterConnection {
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<ParameterConnection> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	boost::weak_ptr<ParameterConnection> self;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Initalisiert Listener.
	 * !NUR AUFRUFEN WENN self-Ptr valid!
	 */
	void initListener();
	//--------------------------------------------------------------------------------------------------------
	void initListener(ConnectionOperator::Ptr op);
	//--------------------------------------------------------------------------------------------------------
	bool updateLock;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (DE)Serialisierung eines ConnectionOperator-Objektes.
	 * @param ar boost::Archive-Objekt.
	 * @param version
	 */
	template < typename Archive >
	void serialize( Archive &ar, const unsigned int version ){
		ar & a;
		ar & b;
		ar & ops;
		ar & self;
		if (Archive::is_loading::value) {
			initListener();
		}
	}
	//--------------------------------------------------------------------------------------------------------
	ParameterPtr a, b;
	//--------------------------------------------------------------------------------------------------------
	ParameterConnection() : updateLock(false) {}
	//--------------------------------------------------------------------------------------------------------
	ParameterConnection(ParameterPtr a, ParameterPtr b);
	//--------------------------------------------------------------------------------------------------------
	typedef ConnectionOperator::Container Operators;
	//--------------------------------------------------------------------------------------------------------
	Operators ops;
	//--------------------------------------------------------------------------------------------------------
	void onOperatorParameterChanged(void *src, const VstNumber &newValue);
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Parameter A ValueChanged-Handler
	 * @param
	 * @param
	 */
	void onChangedA(void *src, const VstNumber &newValue);
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Parameter B ValueChanged-Handler
	 * @param
	 * @param
	 */
	void onChangedB(void *src, const VstNumber &newValue);
public:
	//--------------------------------------------------------------------------------------------------------
	Ptr getPtr() const {
		return self.lock();
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~ParameterConnection();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Operators-Objekt
	 */
	Operators & getOperators() {
		return ops;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return const Operators-Objekt
	 */
	const Operators & getOperators() const {
		return ops;
	}
	//--------------------------------------------------------------------------------------------------------
	void addOperator(ConnectionOperator::Ptr op) {
		ops.push_back(op);
		initListener(op);
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * erstellt ParameterConnection-Objekt
	 * @param Parameter A 
	 * @param Parameter B 
	 */
	static Ptr create(ParameterPtr a, ParameterPtr b) {
		Ptr neu( new ParameterConnection(a,b) );
		neu->self = neu;
		neu->initListener();
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parameter A
	 */
	ParameterPtr getParameterA() const {
		return a;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parameter B
	 */
	ParameterPtr getParameterB() const {
		return b;
	}
	
};
//============================================================================================================
/**
 *  @class ParameterConnectionComparator.
 *  vergleicht zwei ParameterConnection. Wobei Reihenfolge egal ist:  
 *  A<->B oder B<->A ist die gleiche Verbindung.
 */
struct ParameterConnectionComparator : std::binary_function<std::string, std::string, bool> 
{
//============================================================================================================
	//--------------------------------------------------------------------------------------------------------   
	bool operator()(const std::pair<ParameterPtr, ParameterPtr> &r,
        const std::pair<ParameterPtr, ParameterPtr> &l) const
    {
        return ( l.first == r.first &&
			     l.second == r.second ) ||
			   ( l.first == r.second &&
			     l.second == r.first );
    }
};
//============================================================================================================
/**
 *  @class ParameterConnectionSetHash.
 */
struct ParameterConnectionSetHash : std::unary_function<std::string, std::size_t> 
{
//============================================================================================================
	//-------------------------------------------------------------------------------------------------------- 
    std::size_t operator()(const std::pair<ParameterPtr, ParameterPtr> &x) const
    {
        std::size_t seed = 0;
        std::locale locale;
		// create hash by the two target parameters
		std::size_t a = (std::size_t)x.first.get();
		std::size_t b = (std::size_t)x.second.get();
        boost::hash_combine(seed, max(a,b)); // always higher value first
		boost::hash_combine(seed, min(a,b));
        return seed;
    }
};
//============================================================================================================
/**
 * @class ParameterConnectionSet.
 * Container fuer ParameterConnection. Nimmt keine Verbindung doppelt auf. 
 */
class ParameterConnectionSet : 
	public boost::unordered_map<
			std::pair<ParameterPtr, ParameterPtr>,
			ParameterConnection::Ptr, 
			ParameterConnectionSetHash,
			ParameterConnectionComparator>
{
//============================================================================================================
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::unordered_map<
			std::pair<ParameterPtr, ParameterPtr>,
			ParameterConnection::Ptr, 
			ParameterConnectionSetHash,
			ParameterConnectionComparator> Base;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return map key value (parameter A, parameter B)
	 */
	static Base::key_type createKey( ParameterConnection::Ptr cn ) {
		return std::make_pair(cn->getParameterA(), cn->getParameterB());
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verbindet Parameter a mit Parameter b.
	 * @return ParameterConnection, wenn erfolgt. Andernfalls NULL
	 */
	ParameterConnection::Ptr connectParameter(ParameterPtr a, ParameterPtr b) {
		if (!a || !b || a == b)
			return ParameterConnection::Ptr();
		pair<Base::iterator,bool> ret;
		ParameterConnection::Ptr cn = ParameterConnection::create(a,b);
		ret = insert(std::make_pair(createKey(cn), cn));
		return ret.second ? cn : ParameterConnection::Ptr();
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Entfernt ParameterVerbindung a<->b falls vorhanden.
	 * Parameter-Reihenfolge a,b) oder (b,a) spielt keine Rolle. 
	 * @return true, wenn erfolgt
	 */
	bool removeConnection(ParameterPtr a, ParameterPtr b) {
		if (!a || !b)
			return false;
		Base::iterator it = find( std::make_pair(a, b) );
		if (it==end()) 
			return false;
		erase(it);
		return true;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * liefert ParameterVerbindung a<->b falls vorhanden.
	 * Parameter-Reihenfolge a,b) oder (b,a) spielt keine Rolle. 
	 * @return NULL, falls keine Verbindung existiert.
	 */
	ParameterConnection::Ptr getConnection(ParameterPtr a, ParameterPtr b) {
		if (!a || !b)
			return ParameterConnection::Ptr();
		Base::iterator it = find(std::make_pair(a, b));
		if (it==end()) 
			return ParameterConnection::Ptr();
		return it->second;
	}
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (DE)Serialisierung eines ParameterConnectionSet-Objektes.
	 * @param ar boost::Archive-Objekt.
	 * @param version
	 */
	template < typename Archive >
	void save( Archive &ar, const unsigned int version ) const {
		//unordered_set mit spezalisiertem hash-creator(H)
		//und comparator(P) konnte nicht serialisert werden:
		//'serialize': Ist kein Element von 'boost::unordered_set<T,H,P>'
		//ar & boost::serialization::base_object<Base> (*this);
		list<ParameterConnection::Ptr>  l;
		BOOST_FOREACH(const Base::value_type &obj, *this) {
			l.push_back(obj.second);
		}
		ar << l;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (DE)Serialisierung eines ParameterConnectionSet-Objektes.
	 * @param ar boost::Archive-Objekt.
	 * @param version
	 */
	template < typename Archive >
	void load( Archive &ar, const unsigned int version ) {
		//unordered_set mit spezalisiertem hash-creator(H)
		//und comparator(P) konnte nicht serialisert werden:
		//'serialize': Ist kein Element von 'boost::unordered_set<T,H,P>'
		//ar & boost::serialization::base_object<Base> (*this);
		list<ParameterConnection::Ptr>  l;
		ar >> l;
		BOOST_FOREACH(ParameterConnection::Ptr cn, l) {
			insert(std::make_pair(createKey(cn), cn));
		}
	}
};

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
		ValueChangedSender<float>::notifyListeners(this, *this);
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
}
} // namespace parameter
} // namespace processing
#endif



