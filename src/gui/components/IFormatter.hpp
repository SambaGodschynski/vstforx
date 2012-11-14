/*
 * IFormatter.hpp
 *
 *  Created on: Tue Nov  6 21:46:52 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IFORMATTER_H
#define SAMBAG_IFORMATTER_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/Forward.hpp>

namespace frx { namespace gui { namespace components {
namespace sd = sambag::disco;
namespace sdc = sd::components;
//=============================================================================
/** 
  * @class IFormatter. A minimal version of a LayoutManager.
  * Handles where to position several objects. Only useful on containers
  * without LayoutManager.
  */
class IFormatter {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IFormatter> Ptr;
	//-------------------------------------------------------------------------
	virtual void addElement(sdc::AComponentPtr c) = 0;
}; // IFormatter
}}} // namespace(s)

#endif /* SAMBAG_IFORMATTER_H */
