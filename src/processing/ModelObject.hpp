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
#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#include "com/SerializationFwd.h"
#include <boost/serialization/access.hpp>
#include <string>
#include <vector>
#include <set>
#include <list>


namespace frx { namespace processing {
class IModelController;
typedef boost::shared_ptr<IModelController> IModelControllerPtr;
class IParameter;
typedef boost::shared_ptr<IParameter> IParameterPtr;
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
	typedef boost::weak_ptr<ModelObject> WPtr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<void> AnyWPtr;
	//-------------------------------------------------------------------------
	typedef boost::function<bool(Ptr obj)> RequestRemoveFunction;
	//-------------------------------------------------------------------------
	class Connection {
		boost::shared_ptr<bool> disconnected_;
		explicit Connection(boost::shared_ptr<bool> d) : disconnected_(d) {}
	public:
		Connection() {}
		void disconnect() { if (disconnected_) *disconnected_ = true; }
		bool connected() const { return disconnected_ && !*disconnected_; }
		friend class ModelObject;
	};
	//-------------------------------------------------------------------------
	// Lightweight boolean-combining signal replacing boost::signals2
	class Signal {
		struct Slot {
			RequestRemoveFunction fn;
			boost::weak_ptr<void> tracker;
			bool hasTracker;
			boost::shared_ptr<bool> disconnected;
		};
		std::list<Slot> slots;
	public:
		Connection connect(const RequestRemoveFunction &f) {
			boost::shared_ptr<bool> d = boost::make_shared<bool>(false);
			Slot s;
			s.fn = f;
			s.hasTracker = false;
			s.disconnected = d;
			slots.push_back(s);
			return Connection(d);
		}
		Connection connect_tracked(const RequestRemoveFunction &f,
			const boost::weak_ptr<void> &toTrack)
		{
			boost::shared_ptr<bool> d = boost::make_shared<bool>(false);
			Slot s;
			s.fn = f;
			s.tracker = toTrack;
			s.hasTracker = true;
			s.disconnected = d;
			slots.push_back(s);
			return Connection(d);
		}
		size_t num_slots() const {
			size_t count = 0;
			for (typename std::list<Slot>::const_iterator it = slots.begin(); it != slots.end(); ++it) {
				if (!*it->disconnected && (!it->hasTracker || !it->tracker.expired()))
					++count;
			}
			return count;
		}
		// Returns true if all slots return true (AND-combining), false if any returns false.
		// Returns true if no slots connected.
		bool operator()(Ptr obj) {
			for (std::list<Slot>::iterator it = slots.begin(); it != slots.end(); ) {
				if (*it->disconnected) { it = slots.erase(it); continue; }
				if (it->hasTracker && it->tracker.expired()) { it = slots.erase(it); continue; }
				if (!it->fn(obj)) return false;
				++it;
			}
			return true;
		}
	};
protected:
	//-------------------------------------------------------------------------
	WPtr self;
	//-------------------------------------------------------------------------
	Signal signal;
	//-------------------------------------------------------------------------
	virtual bool removeImpl(IModelControllerPtr ctrl) = 0;
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
		ar & self;
	}
public:
    //-------------------------------------------------------------------------
    virtual void setName(const std::string &name) {}
    //-------------------------------------------------------------------------
    virtual std::string getName() const {return "";}
	//-------------------------------------------------------------------------
	/**
	 * calls all connected remove handler.
	 * @return true if object removed from model
	 */
	virtual bool requestRemove();
	//-------------------------------------------------------------------------
	virtual Connection addRemoveRequestExecuter(const RequestRemoveFunction& f)
	{
		return signal.connect(f);
	}
	//-------------------------------------------------------------------------
	virtual Connection addRemoveRequestExecuter(const RequestRemoveFunction& f,
		AnyWPtr toTrack)
	{
		return signal.connect_tracked(f, toTrack);
	}
	//-------------------------------------------------------------------------
	virtual bool remove(IModelControllerPtr ctrl);
	//-------------------------------------------------------------------------
	virtual ~ModelObject() {}
	///////////////////////////////////////////////////////////////////////////
	// Parameter
	typedef std::string ParameterGroupKey;
	typedef std::vector<IParameterPtr> Parameters;
	typedef std::set<ParameterGroupKey> ParameterGroupKeys;
	//-------------------------------------------------------------------------
	virtual void getParameterGroupKeys(ParameterGroupKeys &out) const {};
	//-------------------------------------------------------------------------
	/**
	 * @return parameter by group key. if key == "*" all parameter
	 * will be returned.
	 */
	virtual void getParameters(const ParameterGroupKey &key, Parameters &out) const {};

}; // ModelObject
}} // namespace(s)

#endif /* SAMBAG_MODELOBJECT_H */
