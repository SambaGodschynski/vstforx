/*
 * ===========================================================================================================
 * parameter.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FRX_PARAMETER_HPP
#define FRX_PARAMETER_HPP

#include <vector>
#include <string>
#include <set>
#include <map>
#include "com/one4All.h"
#include "com/Events.h"
#include "processing/PObject.h"
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/foreach.hpp>
#include <unordered_map>
#include <boost/functional/hash.hpp>
namespace processing {
namespace parameter {
//============================================================================================================
// Vorwaertz deklarationen
//============================================================================================================
class ParameterListener;
class HasParameter;
class Parameter;
class ConnectionOperator;
class Inertia;
typedef std::shared_ptr<Parameter> ParameterPtr;
typedef std::shared_ptr<Inertia> InertiaPtr;
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
	typedef std::shared_ptr<HasParameter> Ptr;
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
 *  @interface: HasOutParameter.
 */
class HasOutParameter{
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	typedef std::shared_ptr<HasOutParameter> Ptr;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Parameter zu index.
	 */
	virtual ParameterPtr getOutParameter ( size_t nr = 0 ) const = 0;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller Parameter.
	 */
	virtual size_t getNumOutParameter () const = 0;
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
	typedef std::shared_ptr<ConnectionOperator> Ptr;
	//--------------------------------------------------------------------------------------------------------
	typedef std::list<ConnectionOperator::Ptr> Container; 
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
	std::string name;
protected:
	//--------------------------------------------------------------------------------------------------------
	ConnectionOperator(){}
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Operatorname
	 */
	const std::string & getName() const { return name; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Operatorname
	 * @param _name
	 */
	void setName ( const std::string &_name ) { name = _name; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual com::VstNumber operate ( com::VstNumber f ) = 0;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * implementiert inverse Operation
	 * @param f urspuengl. Parameter Wert
	 * @return Parameterwert nach Operation
	 */
	virtual com::VstNumber operateInverse ( com::VstNumber f ) = 0;
	//--------------------------------------------------------------------------------------------------------
	virtual ~ConnectionOperator() {}
};
//============================================================================================================
/**
 * @class ParameterConnection.
 * Repraesentiert Parameter-Verbindung.
 */
class ParameterConnection : public HasParameter {
//============================================================================================================
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef std::shared_ptr<ParameterConnection> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	std::weak_ptr<ParameterConnection> self;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Initalisiert Listener.
	 * !NUR AUFRUFEN WENN self-Ptr valid!
	 */
	void initListener();
    //--------------------------------------------------------------------------------------------------------
    void initInertiaParameter();
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
	void serialize( Archive &ar, const unsigned int version ) {
		ar & a;
		ar & b;
		ar & ops;
		ar & self;
        if (version > 0) {
            ar & inertiaDuration;
            ar & inertiaType;
        } else {
            initInertiaParameter();
        }
		if (Archive::is_loading::value) {
			initListener();
		}
	}
    //--------------------------------------------------------------------------------------------------------
    typedef std::shared_ptr<void> TweenPtr;
    TweenPtr _tween;
	//--------------------------------------------------------------------------------------------------------
	ParameterPtr a, b;
    //--------------------------------------------------------------------------------------------------------
    ParameterPtr inertiaDuration, inertiaType;
	//--------------------------------------------------------------------------------------------------------
	ParameterConnection() : updateLock(false) {}
	//--------------------------------------------------------------------------------------------------------
	ParameterConnection(ParameterPtr a, ParameterPtr b);
	//--------------------------------------------------------------------------------------------------------
	typedef ConnectionOperator::Container Operators;
	//--------------------------------------------------------------------------------------------------------
	Operators ops;
	//--------------------------------------------------------------------------------------------------------
	void onOperatorParameterChanged(void *src, const com::VstNumber &newValue);
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Parameter A ValueChanged-Handler
	 * @param
	 * @param
	 */
	void onChangedA(void *src, const com::VstNumber &newValue);
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Parameter B ValueChanged-Handler
	 * @param
	 * @param
	 */
	void onChangedB(void *src, const com::VstNumber &newValue);
    //--------------------------------------------------------------------------------------------------------
    void update(ParameterPtr p, const com::VstNumber &newValue);
	//--------------------------------------------------------------------------------------------------------
	void onInertiaDurationChanged(void *src, const float &value);
    //--------------------------------------------------------------------------------------------------------
	void onInertiaTypeChanged(void *src, const float &value);
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
	void removeOperator(ConnectionOperator::Ptr op);
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
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 */
	virtual ParameterPtr getParameter ( size_t index ) const;
    //--------------------------------------------------------------------------------------------------------
	/**
	 * @return 1
	 */
	virtual size_t getNumParameter () const;
};
}} // namespace(s)

BOOST_CLASS_VERSION(processing::parameter::ParameterConnection, 1)
    
namespace processing { namespace parameter {
//============================================================================================================
/**
 *  @class ParameterConnectionComparator.
 *  vergleicht zwei ParameterConnection. Wobei Reihenfolge egal ist:  
 *  A<->B oder B<->A ist die gleiche Verbindung.
 */
struct ParameterConnectionComparator
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
struct ParameterConnectionSetHash
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
		boost::hash_combine(seed, ::com::getMax(a,b)); // always higher value first
		boost::hash_combine(seed, ::com::getMin(a,b));
        return seed;
    }
};
//============================================================================================================
/**
 * @class ParameterConnectionSet.
 * Container fuer ParameterConnection. Nimmt keine Verbindung doppelt auf. 
 */
class ParameterConnectionSet : 
	public std::unordered_map<
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
	typedef std::unordered_map<
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
		std::pair<Base::iterator,bool> ret;
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
		//'serialize': Ist kein Element von 'std::unordered_set<T,H,P>'
		//ar & boost::serialization::base_object<Base> (*this);
		std::list<ParameterConnection::Ptr>  l;
		BOOST_FOREACH(const Base::value_type &obj, *this) {
			l.push_back(obj.second);
		}
		ar & l;
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
		//'serialize': Ist kein Element von 'std::unordered_set<T,H,P>'
		//ar & boost::serialization::base_object<Base> (*this);
		std::list<ParameterConnection::Ptr>  l;
		ar & l;
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
	public com::events::ValueChangedSender<float>, 
	public PObject,
	public com::Serializable
{
//============================================================================================================
friend class boost::serialization::access; 
public:
	//--------------------------------------------------------------------------------------------------------
	typedef std::shared_ptr<Parameter> Ptr;
	//--------------------------------------------------------------------------------------------------------
	typedef std::weak_ptr<Parameter> WPtr;
private:
	//--------------------------------------------------------------------------------------------------------
	typedef Parameter* U; //verbundener Parameter
	//--------------------------------------------------------------------------------------------------------
	com::VstNumber _min, _max;
	//--------------------------------------------------------------------------------------------------------
	// Wert des Parameters;
	com::VstNumber value;
	//--------------------------------------------------------------------------------------------------------
	int nr;
	//--------------------------------------------------------------------------------------------------------
	// fuer VST-Plugin Parameter kommunikation
	int index;
	//--------------------------------------------------------------------------------------------------------
	static int instances;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Parameter Groupname 
	 */
	com::MyString groupname;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * nach VST-SDK:
	 * Stuff text with a string representation
	 * ("0.5", "-3", "PLATE", etc...) of the value of parameter index.
	 */
	com::MyString display;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * nach VST-SDK:
	 * Stuff label with the units in which parameter index is displayed
	 * (i.e. "sec", "dB", "type", etc...).
	 */
	com::MyString label;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisierung von Parameter-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	bool readOnly;
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
	com::Mutex mutex;
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
	typedef com::events::ValueChangedSender<com::VstNumber>::ValueChangedFunction ParameterListenerFunction;
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
	 * setzt Parametername
	 * @param name
	 */
	void setName(const std::string &name);
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return ParameterGroupName
	 */
	const com::MyString & getGroupName() const { return groupname; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return ParameterGroupName
	 */
	void setGroupName(const com::MyString &name) { groupname = name; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parameterwert als String  ("0.5", "-3", "PLATE", etc...)
	 */

	const com::MyString & getLabel() const { return label; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt String-Parameterwert
	 * @param label
	 */
	void setLabel(const com::MyString &label){ Parameter::label = label; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parameterwert
	 */
	com::VstNumber getValue() const { 
		return value;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Parameterwert.
	 * @param v
	 */
	void setValue( com::VstNumber v ){
		if ( updateLock ) return;
		// avoid NaN. problems with serialize and deserialize
		// see: issue #113
		if ( !boost::math::isfinite(v) ) { 
			v = 0;
		}
		value = com::getMin<com::VstNumber>( _max, com::getMax<com::VstNumber>( _min, v ) );
		com::events::ValueChangedSender<float>::notifyListeners(this, *this);
	}
	//--------------------------------------------------------------------------------------------------------
	void operator=(com::VstNumber v){ setValue (v); }
	//--------------------------------------------------------------------------------------------------------
	operator com::VstNumber() { return getValue(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parametereinheit (nach VST-SDK i.e. "sec", "dB", "type", etc...).
	 */
	com::MyString  getDisplay() const {
		if (!display.empty()){
			return display;
		}
		return com::MyString( getValue() );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Parametereinheit (nach VST-SDK i.e. "sec", "dB", "type", etc...).
	 * @param display
	 */
	void setDisplay(const com::MyString &display){ Parameter::display = display; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Minimalwert den Parameter annehmen kann
	 * @param v
	 */
	void setMin ( com::VstNumber v ){ _min = v; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt Maximalwert den Parameter annehmen kann
	 * @param v
	 */
	void setMax ( com::VstNumber v ){ _max = v; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parameter-Minimum
	 */
	com::VstNumber getMin() { return _min; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Parameter-Maximum
	 */
	com::VstNumber getMax() { return _max; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true if parameter isReadOnly
	 */
	bool isReadOnly() const {
		return readOnly;
	}
	//--------------------------------------------------------------------------------------------------------
	/** 
	 * set parameter isReadOnly.
	 * @note: this attribute is for query purpose only, it dosen't effect the setValue() function.
	 */
	void setReadOnly(bool val);
};
//============================================================================================================
// Klasse: Parameter.
// Template defs:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < typename Archiv >
void Parameter::serialize( Archiv &ar, const unsigned int version) {
	ar & boost::serialization::base_object<PObject>(*this);
    if (version<1) {
        // scoped objects will break the archive
        com::MyString name;
        ar & name;
        setName(name);
    }
	ar & groupname;
	ar & label;
	ar & display;
	ar & _min;
	ar & _max;
	ar & value;
	ar & nr;
	ar & index;
	ar & readOnly;
}
} // namespace parameter
} // namespace processing

BOOST_CLASS_VERSION(processing::parameter::Parameter, 1);

#endif // FRX_PARAMETER_HPP



