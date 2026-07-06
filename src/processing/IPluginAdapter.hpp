/*
 * IPluginAdapter.hpp
 *
 *  Created on: Wed Oct 31 17:17:10 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IPLUGINADAPTER_H
#define SAMBAG_IPLUGINADAPTER_H

#include <memory>
#include <string>
#include <sambag/disco/components/Forward.hpp>


namespace sambag { namespace disco { namespace components {
    class AWindowImpl;
    typedef std::shared_ptr<AWindowImpl> AWindowImplPtr;
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
	typedef std::shared_ptr<IPluginAdapter> Ptr;
	typedef std::weak_ptr<IPluginAdapter> WPtr;
    //-------------------------------------------------------------------------
    virtual void beforeOpenEditor(sdc::WindowPtr win) = 0;
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
	virtual std::string getLocation() const = 0;
    //-------------------------------------------------------------------------
    /**
     * @return WindowImpl if the plugin has its own. Can be NULL. 
     * (Bridged plugins have its own impl.)
     */
    virtual sdc::AWindowImplPtr getWindowImpl() = 0;
    //-------------------------------------------------------------------------
    virtual bool isBridged() const = 0;
    //-------------------------------------------------------------------------
    virtual bool isInternal() const = 0;
    //-------------------------------------------------------------------------
    /**
     * @brief sends a message to the plugin.
     *        The behaviour depends on the specific plugin implementatiob.
     */
    virtual std::string sendMessage(const std::string &msg) = 0;

}; // IPluginAdapter
}} // namespace(s)

#endif /* SAMBAG_IPLUGINADAPTER_H */
