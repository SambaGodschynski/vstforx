/*
 * PluginAdapter.hpp
 *
 *  Created on: Wed Oct 31 15:02:15 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PLUGINADAPTER_H
#define SAMBAG_PLUGINADAPTER_H

#include <boost/shared_ptr.hpp>
#include "IPluginAdapter.hpp"
#include "ProcessorAdapter.hpp"
#include "Plugin.h"
#include "Forward.hpp"

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class PluginAdapter.
  */
class PluginAdapter : public ProcessorAdapter, public IPluginAdapter {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<PluginAdapter> Ptr;
protected:
private:
public:
	//-------------------------------------------------------------------------
	static Ptr create(Adaptee::Ptr a = Adaptee::Ptr()) {
		Ptr res(new PluginAdapter());
		res->setAdaptee(a);
		return res;
	}
}; // PluginAdapter
}} // namespace(s)

#endif /* SAMBAG_PLUGINADAPTER_H */
