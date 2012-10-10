/*
 * IEntry.hpp
 *
 *  Created on: Wed Oct 10 14:11:05 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IENTRY_H
#define SAMBAG_IENTRY_H

#include <boost/shared_ptr.hpp>
#include "ModelObject.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class IEntry.
  */
class IEntry : public ModelObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IEntry> Ptr;
}; // IEntry

}} // namespace(s)

#endif /* SAMBAG_IENTRY_H */
