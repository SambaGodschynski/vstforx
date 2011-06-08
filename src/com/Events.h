#ifndef EVENTS_H
#define EVENTS_H

#include <list>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/function_equal.hpp>

using namespace std;

namespace com{
namespace events{
//============================================================================================================
//	Klasse Event:
//============================================================================================================
struct Event;
//============================================================================================================
//	Klasse Listener:
//  Oberklasse fuer alle Listener.
//============================================================================================================
class Listener;
//============================================================================================================
//	Klasse EventListener:
//============================================================================================================
template < class EventType >
class EventListener;
//============================================================================================================
//	Template IValueChangedEvent:
//  Interface fuer alle das ereigniss Value Changed.
//============================================================================================================
template < class T >
class ValueChangedListener;
//============================================================================================================
//	Template ValueChangedSender:
//============================================================================================================
template < class T >
class ValueChangedSender;
//============================================================================================================
//	Template EventSender:
//============================================================================================================
template < class EventType >
class EventSender;
//============================================================================================================
//	Klasse MultiEventSender :
//============================================================================================================
template < typename TLIST >
class MultiEventSender;

} // namespace events
} // namespace com

namespace com{
namespace events{
//============================================================================================================
//	Klasse Event:
//============================================================================================================
struct Event {
	//--------------------------------------------------------------------------------------------------------
	virtual ~Event(){}
	//--------------------------------------------------------------------------------------------------------
	enum EventTypeVerification { verification }; 
};
//============================================================================================================
//	Klasse ValueChangedEvent:
//============================================================================================================
template < typename T >
struct ValueChangedEvent : public Event {
	T value;
	ValueChangedEvent ( const T &value ) : value(value) {}
};
//============================================================================================================
// Event Klasse OnDestroy :
//============================================================================================================
template < typename T >
struct OnDestroy : public Event {
	T *src;
	OnDestroy ( T *src ) : src(src) {}
};
//============================================================================================================
//	Klasse Listener:
//  Oberklasse fuer alle Listener.
//============================================================================================================
class Listener {
	//--------------------------------------------------------------------------------------------------------
public:
	//--------------------------------------------------------------------------------------------------------
	virtual ~Listener(){}
	//--------------------------------------------------------------------------------------------------------
	typedef list<Listener*> ListenerContainer;
};
//============================================================================================================
//	Template IValueChangedEvent:
//  Interface fuer alle das ereigniss Value Changed.
//============================================================================================================
template < class T >
class ValueChangedListener : public Listener {
private:
protected:
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void valueChanged ( void *src, const T &value ) = 0;  
};
//============================================================================================================
//	Template ValueChangedEventSender:
//============================================================================================================
template < class T >
class ValueChangedSender {
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::function< void ( void*, const T& ) > ValueChangedFunction;
	//--------------------------------------------------------------------------------------------------------
	typedef list < ValueChangedListener<T>* > ListenerType;
	//--------------------------------------------------------------------------------------------------------
	typedef list < ValueChangedFunction > ListenerTypeF;
private:
	//--------------------------------------------------------------------------------------------------------
	ListenerType listeners;
	//--------------------------------------------------------------------------------------------------------
	ListenerTypeF funcListeners;
protected:
public:
	//--------------------------------------------------------------------------------------------------------
	void addValueChangedListener ( ValueChangedListener<T> *vCl ) { listeners.push_back (vCl); }
	//--------------------------------------------------------------------------------------------------------
	void removeValueChangedListener ( ValueChangedListener<T> *vCl ) {
		if ( listeners.empty() ) return;
		typename ListenerType::iterator it = listeners.begin();
		for ( ; it!=listeners.end(); ++it ) {
			if ( *it == vCl ) *it = NULL;
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void addValueChangedListenerF ( const ValueChangedFunction &vCl ) { funcListeners.push_back (vCl); }
	//--------------------------------------------------------------------------------------------------------
	void removeValueChangedListenerF ( const ValueChangedFunction &vCl ) { 
		if ( funcListeners.empty() ) return;
		typename list< ValueChangedFunction >::iterator it = funcListeners.begin();
		for ( ; it != funcListeners.end(); ++it ){
			if ( vCl.functor.func_ptr == (*it).functor.func_ptr ){
				*it = NULL; // removing while notify(currency safe)
			}
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void notifyListeners ( void *src, const T &value );
	//--------------------------------------------------------------------------------------------------------
	// notify listeners with the option to skip one function ptr ( e.g. to avoid feedback calls )
	void notifyListeners ( void *src, const T &value, ValueChangedFunction &skipThis );
};
//============================================================================================================
// Template Definitionen:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
template < class T >
void ValueChangedSender<T>::notifyListeners( void *src, const T &value ) {
	//////////listener
	if ( !listeners.empty() ) {
		typename ListenerType::iterator it = listeners.begin();
		while ( it!=listeners.end() ) {
			if (*it) (*it)->valueChanged ( src, value );
			if ( !(*it) ) it = listeners.erase (it);
			else ++it;
		}
	}
	if ( funcListeners.empty() ) return;
	//////////functions
	typename ListenerTypeF::iterator itF = funcListeners.begin();
	while ( itF!=funcListeners.end() ) {
		if (*itF) (*itF)( src, value );
		if ( !(*itF) ) itF = funcListeners.erase (itF);
		else ++itF;
	}
}
//------------------------------------------------------------------------------------------------------------
template < class T >
void ValueChangedSender<T>::notifyListeners( void *src, const T &value, ValueChangedFunction &skipThis ) {
	//////////Objekt Listener 
	if ( !listeners.empty() ) {
		typename ListenerType::iterator it = listeners.begin();
		while ( it!=listeners.end() ) {
			if (*it) (*it)->valueChanged ( src, value );
			if ( !(*it) ) it = listeners.erase (it);
			else ++it;
		}
	}
	if ( funcListeners.empty() ) return;
	//////////Functions Listener 
	typename ListenerTypeF::iterator itF = funcListeners.begin();
	while ( itF!=funcListeners.end() ) {
		if ( itF->functor.func_ptr == skipThis.functor.func_ptr ) {
			++itF;
			continue;
		}
		if (*itF) (*itF)( src, value );
		if ( !(*itF) ) itF = funcListeners.erase (itF);
		else ++itF;
	}
}

//============================================================================================================
//	Klasse EventListener:
//============================================================================================================
template < typename EventType >
class EventListener : public Listener {
private:
	// stellt sicher dass EventType vom Typ Event ist.
	enum { eventTypeVerification = EventType::verification };
protected:
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *src, const EventType &ev ) = 0;  
};
//============================================================================================================
//	Klasse EventSender:
//============================================================================================================
template < typename EventType >
class EventSender {
private:
	//--------------------------------------------------------------------------------------------------------
	// stellt sicher dass EventType vom Typ Event ist.
	enum { eventTypeVerification = EventType::verification };
	//--------------------------------------------------------------------------------------------------------
	typedef list < EventListener<EventType>* > ListenerType;
	//--------------------------------------------------------------------------------------------------------
	typedef boost::function< void ( void*, const EventType& ) > EventFunction;
	//--------------------------------------------------------------------------------------------------------
	typedef list < EventFunction > ListenerTypeF;
	//--------------------------------------------------------------------------------------------------------
	ListenerType listeners;
	//--------------------------------------------------------------------------------------------------------
	ListenerTypeF funcListeners;
protected:
public:
	//--------------------------------------------------------------------------------------------------------
	void addEventListener ( EventListener<EventType> *eL ) { listeners.push_back (eL); }
	//--------------------------------------------------------------------------------------------------------
	void removeEventListener ( EventListener<EventType> *eL ) {
		if ( listeners.empty() ) return;
		typename ListenerType::iterator it = listeners.begin();
		for ( ; it!=listeners.end(); ++it ) {
			if ( *it == eL ) *it = NULL; // removing while notify(currency safe)
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void addEventListenerF ( const EventFunction &f ) { funcListeners.push_back (f); }
	//--------------------------------------------------------------------------------------------------------
	void removeEventListenerF ( const EventFunction &f ) { 
		if ( funcListeners.empty() ) return;
		typename list< EventFunction >::iterator it = funcListeners.begin();
		for ( ; it != funcListeners.end(); ++it ){
			if ( f.functor.func_ptr == (*it).functor.func_ptr ){
				*it = NULL; // removing while notify(currency safe)
			}
		}
	}
	//--------------------------------------------------------------------------------------------------------
	void notifyEventListeners ( void *src, const EventType &ev ) {
		//////////Listener
		if ( listeners.empty() ) return;
		typename ListenerType::iterator it = listeners.begin();
		while ( it!=listeners.end() ) {
			if (*it) 
				(*it)->eventHandler ( src, ev );
			if 
				( !(*it) ) it = listeners.erase (it);
			else
				++it;
		}

		//////////functions 
		typename ListenerTypeF::iterator itF = funcListeners.begin();
		while ( itF!=funcListeners.end() ) {
			if (*itF) 
				(*itF)( src, ev );
			if ( !(*itF) ) 
				itF = funcListeners.erase (itF);
			else 
				++itF;
		}
	}
};


} // namespace events
} // namespace com
#endif


