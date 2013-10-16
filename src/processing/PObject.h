/*
 * ===========================================================================================================
 * PObject.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef _POBJECT_H
#define _POBJECT_H

#include "com/Events.h"
#include "com/Serialization.h"

namespace processing {
class Graph;
//============================================================================================================
/**
 * @class PObject.
 * Oberklasse aller Process-Logik-Objekte
 */
class PObject : public com::events::EventSender< com::events::OnDestroy<PObject> >
{
//============================================================================================================
friend class boost::serialization::access;
friend class Graph;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<PObject> Ptr;
	//--------------------------------------------------------------------------------------------------------
	boost::weak_ptr<PObject> self;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisierung von PObject-Objekten
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template< typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {		
		ar & name;
		ar & self;
		ar & owned;
	} 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * POBject ist in einem Graph enthalten.
	 */
	bool owned;
	//--------------------------------------------------------------------------------------------------------
	std::string name;
	//--------------------------------------------------------------------------------------------------------
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return PObject Pointer
	 */
	Ptr getPtr() const { return self.lock(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn POBject von einem Graph enthalten.
	 */
	bool isOwned() { return owned; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * PObjectname
	 * @return
	 */
	const std::string & getName() const { return name; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt PObject-Name
	 * @param _name
	 */
	void setName ( const std::string &_name ) { name = _name; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~PObject() {
		com::events::EventSender< com::events::OnDestroy<PObject> >::notifyEventListeners ( 
			this, com::events::OnDestroy<PObject>(this) 
		);
	}
	//--------------------------------------------------------------------------------------------------------
	PObject( const std::string &name = "unnamed" ) : name(name), owned(false) {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * wird aufgerufen wenn SampleRate oder BlockSize geaendert wird
	 */
	virtual void hostBaseConfigChanged(){}
};
} // namespace
#endif
