/*
 * INode.hpp
 *
 *  Created on: Sat Oct  6 13:52:49 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_INODE
#define SAMBAG_INODE

#include <memory>
#include "ModelObject.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class INode.
  */
class INode : public ModelObject {
//=============================================================================
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
		ar & boost::serialization::base_object<ModelObject> ( *this );
	}
public:
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<INode> Ptr;
	//-------------------------------------------------------------------------
	virtual ~INode() {}
}; // INode
}} // namespace(s)

#endif /* SAMBAG_INODE */
