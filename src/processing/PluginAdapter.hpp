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
	//-------------------------------------------------------------------------
	typedef Plugin Adaptee;
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
		ar & boost::serialization::base_object<ProcessorAdapter> ( *this );
	}
protected:
	//-------------------------------------------------------------------------
	Adaptee::Ptr getPlugin() const {
		return boost::dynamic_pointer_cast<Adaptee>(getAdaptee());
	}
private:
public:
	//-------------------------------------------------------------------------
	static Ptr create(Adaptee::Ptr a = Adaptee::Ptr()) {
		Ptr res(new PluginAdapter());
		res->setAdaptee(a);
		res->self = res;
		return res;
	}
	//-------------------------------------------------------------------------
	virtual bool isSynth() const;
	//-------------------------------------------------------------------------
	virtual void openEditor(sdc::WindowPtr win);
	//-------------------------------------------------------------------------
	virtual void closeEditor(sdc::WindowPtr win);
    //-------------------------------------------------------------------------
	virtual bool hasEditor() const;
	//-------------------------------------------------------------------------
	virtual void onEditorIdle();
	//-------------------------------------------------------------------------
	virtual std::string getName() const;
	//-------------------------------------------------------------------------
	virtual size_t getNumPresets() const;
	//-------------------------------------------------------------------------
	virtual std::string getPresetName(size_t i) const;
	//-------------------------------------------------------------------------
	virtual void setPreset(int i);
    //-------------------------------------------------------------------------
    /**
     * @return WindowImpl if the plugin has its own. Can be NULL. 
     * (Bridged plugins have its own impl.)
     */
    virtual sdc::AWindowImplPtr getWindowImpl();
    //-------------------------------------------------------------------------
    virtual bool isBridged() const;
}; // PluginAdapter
}} // namespace(s)

#endif /* SAMBAG_PLUGINADAPTER_H */
