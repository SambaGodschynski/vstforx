/*
 * ===========================================================================================================
 * Events.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef EVENTS_H
#define EVENTS_H

#include <list>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/signals2.hpp>
#include <boost/smart_ptr.hpp>

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
//	Template ValueChangedSender:
//============================================================================================================
template < class T >
class ValueChangedSender;
//============================================================================================================
//	Template EventSender:
//============================================================================================================
template < class EventType >
class EventSender;

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
//	Template ValueChangedEventSender:
//============================================================================================================
template < class T >
class ValueChangedSender {
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::function< void ( void*, const T& ) > ValueChangedFunction;
	//--------------------------------------------------------------------------------------------------------
	typedef boost::signals2::signal< void ( void*, const T& ) > Signal;
	//--------------------------------------------------------------------------------------------------------
	typedef boost::signals2::connection Connection;
private:
	//--------------------------------------------------------------------------------------------------------
	Signal signal;
protected:
public:
	//--------------------------------------------------------------------------------------------------------
	Connection addValueChangedListener ( const ValueChangedFunction &vCl ) { 
		return signal.connect(vCl);
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt Listener hinzu und aktiviert tracking.
	 * @see http://www.boost.org/doc/libs/1_40_0/doc/html/signals2/tutorial.html#id1664686 
	 * Section: Automatic Connection Management (Intermediate)
	 * @param 
	 * @param weak pointer zum zu trackenden Objekt
	 */
	Connection addTrackedValueChangedListener ( const ValueChangedFunction &vCl,
		const boost::weak_ptr<void> &toTrack ) 
	{ 
		return signal.connect(
			Signal::slot_type(vCl).track(toTrack)
		);
	}
	//--------------------------------------------------------------------------------------------------------
	void notifyListeners ( void *src, const T &value ) {
		signal( src, value );
	}
};
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
class EventSender  {
public:
	//--------------------------------------------------------------------------------------------------------
	typedef typename ValueChangedSender<EventType>::Connection EventConnection;
	//--------------------------------------------------------------------------------------------------------
	typedef typename ValueChangedSender<EventType>::Signal EventSignal;
private:
	//--------------------------------------------------------------------------------------------------------
	// nicht beerben sonst mehrdeudikeitsprobleme!
	ValueChangedSender<EventType> sender;
	//--------------------------------------------------------------------------------------------------------
	// stellt sicher dass EventType vom Typ Event ist.
	enum { eventTypeVerification = EventType::verification };
public:
	//--------------------------------------------------------------------------------------------------------
	EventConnection addEventListener ( EventListener<EventType> *eL ) { 
		return sender.addValueChangedListener(
			boost::bind( &EventListener<EventType>::eventHandler, eL, _1, _2)
		);	
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt Listener hinzu und aktiviert tracking.
	 * @see http://www.boost.org/doc/libs/1_40_0/doc/html/signals2/tutorial.html#id1664686 
	 * Section: Automatic Connection Management (Intermediate)
	 * @param 
	 * @param weak pointer zum zu trackenden Objekt
	 */
	typename EventConnection addTrackedEventListener ( EventListener<EventType> *eL,
		const boost::weak_ptr<void> &toTrack ) 
	{ 
		return sender.addTrackedValueChangedListener(
			boost::bind( typename &EventListener<EventType>::eventHandler, eL, _1, _2),
			toTrack
		);	
	}
	//--------------------------------------------------------------------------------------------------------
	void notifyEventListeners( void *src, const EventType &ev ) {
		sender.notifyListeners(src, ev);
	}
};


} // namespace events
} // namespace com
#endif


