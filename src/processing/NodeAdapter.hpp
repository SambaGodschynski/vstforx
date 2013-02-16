/*
 * NodeAdapter.hpp
 *
 *  Created on: Thu Oct 18 13:31:59 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_NODEADAPTER_H
#define SAMBAG_NODEADAPTER_H

#include <boost/shared_ptr.hpp>
#include <sambag/com/Common.hpp>
#include "INode.hpp"
#include "processing.h"
namespace frx { namespace processing {
//=============================================================================
/** 
  * @class NodeAdapter.
  */
class NodeAdapter : public INode {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef INode Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<NodeAdapter> Ptr;
	//-------------------------------------------------------------------------
	typedef ::processing::ProcessorNode Adaptee;
protected:
	//-------------------------------------------------------------------------
	NodeAdapter(){}
	//-------------------------------------------------------------------------
	Adaptee::Ptr node;
	//-------------------------------------------------------------------------
	virtual bool removeImpl(IModelControllerPtr ctrl) {
		return true;
	}
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
		ar & boost::serialization::base_object<INode> ( *this );
		ar & node;
	}
public:
	//-------------------------------------------------------------------------
	Ptr getPtr() const {
		return boost::shared_dynamic_cast<NodeAdapter>( self.lock() );
	}
	//-------------------------------------------------------------------------
	static Ptr create(Adaptee::Ptr a = Adaptee::Ptr()) {
		Ptr res(new NodeAdapter());
		res->setAdaptee(a);
		res->self = res;
		return res;
	}
	//-------------------------------------------------------------------------
	void setAdaptee(Adaptee::Ptr n) {
		node = n;
	}
	//-------------------------------------------------------------------------
	Adaptee::Ptr getAdaptee() const {
		return node;
	}
protected:
private:
public:
}; // NodeAdapter
}} // namespace(s)

#endif /* SAMBAG_NODEADAPTER_H */
