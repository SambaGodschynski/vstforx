/*
 * IOutput.hpp
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
  * @class IOutput.
  */
class IOutput : public ModelObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IOutput> Ptr;
}; // IOutput
}} // namespace(s)

#endif /* SAMBAG_IOUTPUT_H */
