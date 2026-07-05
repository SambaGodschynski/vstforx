/*
 * ===========================================================================================================
 * Events.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef EVENTS_H
#define EVENTS_H

#include <list>
#include <functional>
#include <memory>

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
	typedef std::shared_ptr<TrackingDummy> Ptr;
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
	typedef std::list<Listener*> ListenerContainer;
};
//============================================================================================================
/**
 * @class Connection.
 * Lightweight replacement for boost::signals2::connection.
 * Shared state between the handle and the slot ensures the slot remains
 * connected as long as disconnect() has not been called, regardless of
 * whether the Connection handle itself is kept alive.
 */
class Connection {
	struct State {
		bool disconnected;
		int  blocked;
		State() : disconnected(false), blocked(0) {}
	};
	std::shared_ptr<State> state_;
	explicit Connection(std::shared_ptr<State> s) : state_(s) {}
public:
	Connection() {}
	void disconnect()  { if (state_) state_->disconnected = true; }
	bool connected()   const { return state_ && !state_->disconnected; }
	bool is_blocked()  const { return state_ &&  state_->blocked > 0; }
	// Internal for ScopedBlock
	void _block()   { if (state_) ++state_->blocked; }
	void _unblock() { if (state_) --state_->blocked; }
	template<class T> friend class ValueChangedSender;
};
//============================================================================================================
/**
 * @class ScopedBlock.
 * RAII guard that temporarily blocks a connection (like signals2::shared_connection_block).
 * While blocked, notifyListeners silently skips the slot without removing it.
 */
class ScopedBlock {
	Connection cn_;
public:
	explicit ScopedBlock(Connection &cn) : cn_(cn) { cn_._block(); }
	~ScopedBlock() { cn_._unblock(); }
};
//============================================================================================================
/*
 * @class ValueChangedEventSender.
 * Lightweight replacement for boost::signals2::signal<void(void*, const T&)>.
 */
template < class T >
class ValueChangedSender {
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	typedef std::function< void ( void*, const T& ) > ValueChangedFunction;
	//--------------------------------------------------------------------------------------------------------
	typedef ::com::events::Connection Connection;
private:
	//--------------------------------------------------------------------------------------------------------
	struct Slot {
		ValueChangedFunction fn;
		std::weak_ptr<void> tracker;
		bool hasTracker;
		std::shared_ptr<Connection::State> state;
	};
	std::list<Slot> slots;
protected:
public:
	//--------------------------------------------------------------------------------------------------------
	Connection addValueChangedListener ( const ValueChangedFunction &vCl ) {
		std::shared_ptr<Connection::State> s = std::make_shared<Connection::State>();
		Slot slot;
		slot.fn = vCl;
		slot.hasTracker = false;
		slot.state = s;
		slots.push_back(slot);
		return Connection(s);
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt Listener hinzu und aktiviert tracking.
	 * Slot wird automatisch entfernt wenn toTrack ablaeuft.
	 */
	Connection addTrackedValueChangedListener ( const ValueChangedFunction &vCl,
		const std::weak_ptr<void> &toTrack )
	{
		std::shared_ptr<Connection::State> s = std::make_shared<Connection::State>();
		Slot slot;
		slot.fn = vCl;
		slot.tracker = toTrack;
		slot.hasTracker = true;
		slot.state = s;
		slots.push_back(slot);
		return Connection(s);
	}
	//--------------------------------------------------------------------------------------------------------
	void notifyListeners ( void *src, const T &value ) {
		for (typename std::list<Slot>::iterator it = slots.begin(); it != slots.end(); ) {
			if (it->state->disconnected) {
				it = slots.erase(it);
				continue;
			}
			if (it->hasTracker && it->tracker.expired()) {
				it = slots.erase(it);
				continue;
			}
			if (!it->state->blocked) {
				it->fn(src, value);
			}
			++it;
		}
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
			[eL](void *src, const EventType &ev){ eL->eventHandler(src, ev); }
		);
	}
	//--------------------------------------------------------------------------------------------------------
	EventConnection addTrackedEventListener ( EventListener<EventType> *eL,
		const std::weak_ptr<void> &toTrack )
	{
		return sender.addTrackedValueChangedListener(
			[eL](void *src, const EventType &ev){ eL->eventHandler(src, ev); },
			toTrack
		);
	}
	//--------------------------------------------------------------------------------------------------------
	EventConnection addTrackedEventListener ( const typename Base::ValueChangedFunction &f,
		const std::weak_ptr<void> &toTrack )
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


