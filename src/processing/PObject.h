#ifndef _POBJECT_H
#define _POBJECT_H

#include "com/Events.h"
#include "com/Serialization.h"

namespace processing {
class Graph;
using namespace com;
using namespace events;
//============================================================================================================
//	Klasse: PObject.
//  Jedes PObject darf nur von einem Graph besessen werden.
//============================================================================================================
class PObject : public EventSender< OnDestroy<PObject> > {
friend class boost::serialization::access;
friend class Graph;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<PObject> Ptr;
	//--------------------------------------------------------------------------------------------------------
	boost::weak_ptr<PObject> self;
private:
	//--------------------------------------------------------------------------------------------------------
	template< typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {		
		ar & name;
		ar & self;
		ar & owned;
	} 
	//--------------------------------------------------------------------------------------------------------
	bool owned;
	//--------------------------------------------------------------------------------------------------------
	string name;
	//--------------------------------------------------------------------------------------------------------
public:
	//--------------------------------------------------------------------------------------------------------
	Ptr getPtr() { return self.lock(); }
	//--------------------------------------------------------------------------------------------------------
	bool isOwned() { return owned; }
	//--------------------------------------------------------------------------------------------------------
	const string & getName() const { return name; }
	//--------------------------------------------------------------------------------------------------------
	void setName ( const string &_name ) { name = _name; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~PObject() {
		EventSender< OnDestroy<PObject> >::notifyEventListeners ( this, OnDestroy<PObject>(this) );
	}
	//--------------------------------------------------------------------------------------------------------
	PObject( const string &name = "unnamed" ) : name(name), owned(false) {}
	//--------------------------------------------------------------------------------------------------------
	// wird aufgerufen wenn SampleRate oder BlockSize geaendert wird
	virtual void hostInfoChanged(){}
};
} // namespace
#endif