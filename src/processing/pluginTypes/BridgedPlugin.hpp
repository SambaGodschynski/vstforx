/*
 * BridgedPlugin.hpp
 *
 *  Created on: Mon Jan  6 11:53:25 2014
 *      Author: Johannes Unger
 */


#if 0

#ifndef SAMBAG_BRIDGEDPLUGIN_H
#define SAMBAG_BRIDGEDPLUGIN_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <processing/Plugin.h>
#include <com/Serialization.h>
#include <processing/parameter/Parameter.h>
#include <processing/ModelFactory.hpp>

namespace frx { namespace processing {
namespace oldPr = ::processing;
class PluginSessionClient;
typedef boost::shared_ptr<PluginSessionClient> PluginSessionClientPtr;
//=============================================================================
/**
 * @class BridgedPlugin.
 * @brief plugin proxy using a PluginSession 
 */
class BridgedPlugin : public ::processing::Plugin
{
//=============================================================================
friend class boost::serialization::access;
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<BridgedPlugin> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<BridgedPlugin> WPtr;
private:
	//-------------------------------------------------------------------------
	template <typename Archive>
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & boost::serialization::base_object<Plugin> ( *this );
		if ( Archive::is_loading::value ) {
            restoreSession();
			initListener();
		}
	}
protected:
    //-------------------------------------------------------------------------
    void createSession();
    //-------------------------------------------------------------------------
    void restoreSession();
    //-------------------------------------------------------------------------
    PluginSessionClientPtr session;
	//-------------------------------------------------------------------------
	void initListener();
	//-------------------------------------------------------------------------
	BridgedPlugin() {}
	//-------------------------------------------------------------------------
	BridgedPlugin(IHostInfo::Ptr hostInfo, const std::string &location);
public:
    //-------------------------------------------------------------------------
    static Ptr create(IHostInfo::Ptr hI, const std::string &location);
    //-------------------------------------------------------------------------
    virtual void process(const oldPr::Frames &_in,
        oldPr::Frames &_out, size_t numSamples)
    {
    }
    //-------------------------------------------------------------------------
	virtual size_t getNumPrograms();
	//-------------------------------------------------------------------------
	virtual std::string getProgramName( size_t index );
	//-------------------------------------------------------------------------
	virtual void setProgram( size_t index );
	//-------------------------------------------------------------------------
	virtual int getProgram();
	//-------------------------------------------------------------------------
	virtual bool isAccessable();
	//-------------------------------------------------------------------------
	virtual bool canHandleMidiEvent() const;
	//-------------------------------------------------------------------------
	virtual void processAdapter( oldPr::Processor::Int numSamples );
	//-------------------------------------------------------------------------
	virtual void processMidiEvents( sambag::dsp::IMidiEvents * events );
	//-------------------------------------------------------------------------
	virtual ~BridgedPlugin();
	//-------------------------------------------------------------------------
	virtual bool hasEditor() const;
	//-------------------------------------------------------------------------
	virtual void openEditor(void *window);
	//-------------------------------------------------------------------------
	virtual void closeEditor(void *window);
	//-------------------------------------------------------------------------
	virtual void onEditorIdle();
    //-------------------------------------------------------------------------
    virtual size_t getNumInputChannels() const;
    //-------------------------------------------------------------------------
    virtual size_t getNumOutputChannels() const;
    //-------------------------------------------------------------------------
    virtual ::processing::parameter::ParameterPtr getParameter ( size_t nr = 0 ) const;
    //-------------------------------------------------------------------------
    virtual size_t getNumParameter () const;
};

namespace {
const bool BridgedPluginRegistered =
    ModelFactory::instance().registerWithDetail<BridgedPlugin>(
        std::string("bridged-plugin.Plugin"),
        &BridgedPlugin::create
    );
}

}} // namespace(s)

#endif /* SAMBAG_BRIDGEDPLUGIN_H */



#endif

