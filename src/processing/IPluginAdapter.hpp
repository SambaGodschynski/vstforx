/*
 * IPluginAdapter.hpp
 *
 *  Created on: Wed Oct 31 17:17:10 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IPLUGINADAPTER_H
#define SAMBAG_IPLUGINADAPTER_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/Forward.hpp>

namespace frx { namespace processing {
namespace sd = sambag::disco;
namespace sdc = sd::components;
//=============================================================================
/** 
  * @class IPluginAdapter.
  */
class IPluginAdapter {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IPluginAdapter> Ptr;
	//-------------------------------------------------------------------------
	virtual void openEditor(sdc::WindowPtr win) = 0;
	//-------------------------------------------------------------------------
	virtual void closeEditor(sdc::WindowPtr win) = 0;
	//-------------------------------------------------------------------------
	virtual bool isSynth() const = 0;
	//-------------------------------------------------------------------------
	virtual std::string getStatusMessage() const = 0;
	//-------------------------------------------------------------------------
	virtual void onEditorIdle() = 0;
	//-------------------------------------------------------------------------
	virtual std::string getName() const = 0;
}; // IPluginAdapter
}} // namespace(s)

#endif /* SAMBAG_IPLUGINADAPTER_H */
