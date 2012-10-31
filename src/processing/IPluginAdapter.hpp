/*
 * IPluginAdapter.hpp
 *
 *  Created on: Wed Oct 31 17:17:10 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IPLUGINADAPTER_H
#define SAMBAG_IPLUGINADAPTER_H

#include <boost/shared_ptr.hpp>

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class IPluginAdapter.
  */
class IPluginAdapter {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IPluginAdapter> Ptr;
}; // IPluginAdapter
}} // namespace(s)

#endif /* SAMBAG_IPLUGINADAPTER_H */
