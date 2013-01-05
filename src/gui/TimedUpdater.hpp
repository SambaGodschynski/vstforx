/*
 * TimedUpdater.hpp
 *
 *  Created on: Sat Jan  5 13:45:21 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PARAMETEROBSERVER_H
#define SAMBAG_PARAMETEROBSERVER_H

#include <loki/Singleton.h>
#include <sambag/disco/components/Timer.hpp>
#include <set>
#include <sambag/com/Thread.hpp>

namespace frx { namespace gui {
//=============================================================================
/** 
  * @class TimedUpdater.
  * updates chaged value at appropriate time.
  *
  * DoUpdatePolicy concept:
  * 
  * DoUpdatePolicy {
  *		void update( const Value &v );
  * };
  */
template <
	class Value, 
	template <class> class _DoUpdatePolicy,
	int _RefreshTime
>
class TimedUpdater : public _DoUpdatePolicy<Value> {
//=============================================================================
friend struct Loki::CreateUsingNew<TimedUpdater>;
public:
	//-------------------------------------------------------------------------
	enum { RefreshTime = _RefreshTime };
	//-------------------------------------------------------------------------
	typedef Value ValueType;
	//-------------------------------------------------------------------------
	typedef _DoUpdatePolicy<ValueType> DoUpdatePolicy;
	//-------------------------------------------------------------------------
	typedef std::set<ValueType> Values;
	//-------------------------------------------------------------------------
	typedef TimedUpdater<Value, _DoUpdatePolicy, RefreshTime> Class;
protected:
	//-------------------------------------------------------------------------
	void doUpdate(void *, const sambag::disco::components::TimerEvent &ev);
private:
	//-------------------------------------------------------------------------
	sambag::com::Mutex mutex;
	//-------------------------------------------------------------------------
	void initTimer();
	//-------------------------------------------------------------------------
	sambag::disco::components::Timer::Ptr refreshTimer;
	//-------------------------------------------------------------------------
	Values values;
	//-------------------------------------------------------------------------
	TimedUpdater(){}
public:
	//-------------------------------------------------------------------------
	void update(const ValueType &data) {
		{
			SAMBAG_TRY_TO_LOCK_TIMED(mutex)
			values.insert(data);
		}
		if (!refreshTimer) {
			initTimer();
		}
	}
	//-------------------------------------------------------------------------
	static TimedUpdater & instance() {
		typedef Loki::SingletonHolder<TimedUpdater> Holder;
		return Holder::Instance();
	}
}; // TimedUpdater
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <
	class V, 
	template <class> class P,
	int T
>
void TimedUpdater<V,P,T>::initTimer() {
	using namespace sambag::disco::components;
	refreshTimer = Timer::create(RefreshTime);
	refreshTimer->sce::EventSender<TimerEvent>::addEventListener(
		boost::bind(&Class::doUpdate, this, _1, _2)
	);
	refreshTimer->setNumRepetitions(-1);
	refreshTimer->start();
}
//-----------------------------------------------------------------------------
template <
	class V, 
	template <class> class P,
	int T
>
void TimedUpdater<V,P,T>::doUpdate(void *, 
	const sambag::disco::components::TimerEvent &ev)
{
	using namespace frx::processing;
	while (!values.empty()) {
		ValueType v;
		{
			SAMBAG_TRY_TO_LOCK_TIMED(mutex)
			Values::iterator it = values.begin();
			v = *it;
			values.erase(it);
		}
		DoUpdatePolicy::update(v);
	}
}
}} // namespace(s)

#endif /* SAMBAG_PARAMETEROBSERVER_H */
