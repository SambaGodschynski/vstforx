/*
 * INode.hpp
 *
 *  Created on: Sat Oct  6 13:52:49 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IOUTPUT_H
#define SAMBAG_IOUTPUT_H

#include <boost/shared_ptr.hpp>
#include "ModelObject.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class INode.
  */
class INode : public ModelObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<INode> Ptr;
	//-------------------------------------------------------------------------
	virtual ~INode() {}
}; // INode
}} // namespace(s)

#endif /* SAMBAG_IOUTPUT_H */
