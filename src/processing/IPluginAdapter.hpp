/*
 * IPluginAdapter.hpp
 *
 *  Created on: Wed Oct 31 17:17:10 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IPLUGINADAPTER_H
#define SAMBAG_IPLUGINADAPTER_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/disco/components/Forward.hpp>


namespace sambag { namespace disco { namespace components {
    class AWindowImpl;
    typedef boost::shared_ptr<AWindowImpl> AWindowImplPtr;
}}}

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
	typedef boost::weak_ptr<IPluginAdapter> WPtr;
	//-------------------------------------------------------------------------
	virtual void openEditor(sdc::WindowPtr win) = 0;
	//-------------------------------------------------------------------------
	virtual void closeEditor(sdc::WindowPtr win) = 0;
	//-------------------------------------------------------------------------
	virtual bool hasEditor() const = 0;
	//-------------------------------------------------------------------------
	virtual void onEditorIdle() = 0;
	//-------------------------------------------------------------------------
	virtual bool isSynth() const = 0;
	//-------------------------------------------------------------------------
	virtual std::string getName() const = 0;
    //-------------------------------------------------------------------------
    /**
     * @return WindowImpl if the plugin has its own. Can be NULL. 
     * (Bridged plugins have its own impl.)
     */
    virtual sdc::AWindowImplPtr getWindowImpl() = 0;
    //-------------------------------------------------------------------------
    virtual bool isBridged() const = 0;
}; // IPluginAdapter
}} // namespace(s)

#endif /* SAMBAG_IPLUGINADAPTER_H */
