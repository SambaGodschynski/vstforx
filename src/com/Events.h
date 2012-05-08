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
/**
 * @class Event: 
 */
struct Event {
//============================================================================================================
	//--------------------------------------------------------------------------------------------------------
	virtual ~Event(){}
	//--------------------------------------------------------------------------------------------------------
	enum EventTypeVerification { verification }; 
};
//============================================================================================================
/**
 * @class ValueChangedEvent:
 */
template < typename T >
struct ValueChangedEvent : public Event {
//============================================================================================================
	T value;
	ValueChangedEvent ( const T &value ) : value(value) {}
};
//============================================================================================================
/** 
 * @class OnDestroy :
 */
template < typename T >
struct OnDestroy : public Event {
//============================================================================================================
	T *src;
	OnDestroy ( T *src ) : src(src) {}
};
//============================================================================================================
/** 
 * @class TrackingDummy.
 * Kann fuer signal::track verwendet werden.
 */
struct TrackingDummy {
//============================================================================================================
	typedef boost::shared_ptr<TrackingDummy> Ptr;
	static Ptr create() {
		return Ptr( new TrackingDummy() );
	}
	virtual ~TrackingDummy(){}
};
//============================================================================================================
/**	
 * @class Listener:
 * Oberklasse fuer alle Listener.
 */
//============================================================================================================
class Listener {
//============================================================================================================
	//--------------------------------------------------------------------------------------------------------
public:
	//--------------------------------------------------------------------------------------------------------
	virtual ~Listener(){}
	//--------------------------------------------------------------------------------------------------------
	typedef list<Listener*> ListenerContainer;
};
//============================================================================================================
/*
 * @class ValueChangedEventSender.
 */
template < class T >
class ValueChangedSender {
//============================================================================================================
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
	// kann nicht sicher impl. werden :
	// http://www.boost.org/doc/libs/1_49_0/doc/html/function/faq.html 
	// Why can't I compare boost::function objects with operator== or operator!=?
	// Alternative:
	// addTrackedValueChangedListener UND ggf. TrackingDummy
	// void removealueChangedListener ( const ValueChangedFunction &vCl ) {}
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
			typename Signal::slot_type(vCl).track(toTrack)
		);
	}
	//--------------------------------------------------------------------------------------------------------
	void notifyListeners ( void *src, const T &value ) {
		signal( src, value );
	}
};
//============================================================================================================
/**	
 * @class EventListener:
 */
template < typename EventType >
class EventListener : public Listener {
//============================================================================================================
private:
protected:
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void eventHandler ( void *src, const EventType &ev ) = 0;  
};
//============================================================================================================
/**
 * @class EventSender.
 */
template < typename EventType >
class EventSender  {
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	typedef typename ValueChangedSender<EventType>::Connection EventConnection;
	//--------------------------------------------------------------------------------------------------------
	typedef typename ValueChangedSender<EventType>::Signal EventSignal;
private:
	//--------------------------------------------------------------------------------------------------------
	typedef ValueChangedSender<EventType> Base;
	//--------------------------------------------------------------------------------------------------------
	// nicht beerben sonst mehrdeudikeitsprobleme!
	ValueChangedSender<EventType> sender;
	//--------------------------------------------------------------------------------------------------------
	// stellt sicher dass EventType vom Typ Event ist.
	enum { eventTypeVerification = EventType::verification };
public:
	//--------------------------------------------------------------------------------------------------------
	EventConnection addEventListener ( const typename Base::ValueChangedFunction &f ) { 
		return sender.addValueChangedListener(f);	
	}
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
	EventConnection addTrackedEventListener ( EventListener<EventType> *eL,
		const boost::weak_ptr<void> &toTrack ) 
	{ 
		return sender.addTrackedValueChangedListener(
			boost::bind(&EventListener<EventType>::eventHandler, eL, _1, _2),
			toTrack
		);	
	}
	//--------------------------------------------------------------------------------------------------------
	EventConnection addTrackedEventListener ( const typename Base::ValueChangedFunction &f,
		const boost::weak_ptr<void> &toTrack ) 
	{ 
		return sender.addTrackedValueChangedListener(f, toTrack);	
	}
	//--------------------------------------------------------------------------------------------------------
	void notifyEventListeners( void *src, const EventType &ev ) {
		sender.notifyListeners(src, ev);
	}
};


} // namespace events
} // namespace com
#endif


