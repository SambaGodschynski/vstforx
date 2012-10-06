/*
 * IInput.hpp
 *
 *  Created on: Sat Oct  6 13:52:36 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IINPUT_H
#define SAMBAG_IINPUT_H

#include <boost/shared_ptr.hpp>
#include "ModelObject.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class IInput.
  */
class IInput : public ModelObject {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IInput> Ptr;
protected:
private:
public:
}; // IInput
}} // namespace(s)

#endif /* SAMBAG_IINPUT_H */
