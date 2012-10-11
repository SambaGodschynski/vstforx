/*
 * ModelObject.hpp
 *
 *  Created on: Fri Oct  5 13:39:30 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_MODELOBJECT_H
#define SAMBAG_MODELOBJECT_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <boost/signals2.hpp>

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class BooleanCombiner.
  * Combiner for boost::signal. Returns res1 && res2 & ... resN
  */
//=============================================================================
struct BooleanCombiner {
	typedef bool result_type; 
	template <typename It> 
	bool operator()(It first, It last) const {
		for (It it=first; it!=last; ++it) {
			if (*it == false) {
				return false;
			}
		}
		return true;
	} 
};
//=============================================================================
/** 
  * @class ModelObject.
  */
class ModelObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ModelObject> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<void> AnyWPtr;
	//-------------------------------------------------------------------------
	typedef boost::function<bool(Ptr obj)> RequestRemoveFunction;
	//-------------------------------------------------------------------------
	typedef boost::signals2::signal<bool(Ptr obj), BooleanCombiner> Signal;
	//-------------------------------------------------------------------------
	typedef boost::signals2::connection Connection;
protected:
	//-------------------------------------------------------------------------
	Signal signal;
public:
	//-------------------------------------------------------------------------
	virtual Connection addRemoveRequestExecuter(const RequestRemoveFunction& f)
	{
		return signal.connect(f);
	}
	//-------------------------------------------------------------------------
	virtual Connection addRemoveRequestExecuter(const RequestRemoveFunction& f,
		AnyWPtr toTrack)
	{
		return signal.connect(
			Signal::slot_type(f).track(toTrack)
		);
	}
	//-------------------------------------------------------------------------
	/**
	 * @param shared_ptr is needed because we can't create one with "this". 
	 * @return true if object removed from model
	 */
	virtual bool requestRemove(Ptr self) {
		return signal(self);
	}
	//-------------------------------------------------------------------------
	virtual ~ModelObject() {}

}; // ModelObject
}} // namespace(s)

#endif /* SAMBAG_MODELOBJECT_H */
