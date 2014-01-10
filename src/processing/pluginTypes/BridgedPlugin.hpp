/*
 * BridgedPlugin.hpp
 *
 *  Created on: Mon Jan  6 11:53:25 2014
 *      Author: Johannes Unger
 */


#ifndef SAMBAG_BRIDGEDPLUGIN_H
#define SAMBAG_BRIDGEDPLUGIN_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "PluginImpl.hpp"
#include <processing/interprocess/PluginSession.hpp>

namespace frx { namespace processing {
namespace oldPr = ::processing;
class PluginSessionClient;
typedef boost::shared_ptr<PluginSessionClient> PluginSessionClientPtr;
//=============================================================================
/**
 * @class BridgedPlugin.
 * @brief plugin proxy using a PluginSession 
 */
class BridgedPlugin : public APluginImpl
{
//=============================================================================
public:
    //-------------------------------------------------------------------------
    BridgedPlugin(IHostInfo::Ptr hI, const std::string &location,
        Parameters *parameters);
    //-------------------------------------------------------------------------
    virtual ~BridgedPlugin();
    //-------------------------------------------------------------------------
    virtual void baseConfigChanged();
    //-------------------------------------------------------------------------
    virtual void turnOff();
    //-------------------------------------------------------------------------
    virtual void turnOn();
    //-------------------------------------------------------------------------
    virtual void openPlugin();
    //-------------------------------------------------------------------------
    virtual void closePlugin();
    //-------------------------------------------------------------------------
    virtual size_t getNumInputChannels() const;
    //-------------------------------------------------------------------------
    virtual size_t getNumOutputChannels() const;
	//-------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin ueber Editor verfuegt.
	 */
	virtual bool hasEditor() const;
	//-------------------------------------------------------------------------
	virtual void openEditor(void *window);
	//-------------------------------------------------------------------------
	virtual void closeEditor(void *window);
	//-------------------------------------------------------------------------
	virtual void onEditorIdle();
    //-------------------------------------------------------------------------
    virtual bool isAccessable() const;
	//-------------------------------------------------------------------------
	/**
	 * @return Anzahl aller Plugin-Programme (aka. Presets)
	 */
	virtual size_t getNumPrograms();
	//-------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Program-Name zu index.
	 */
	virtual std::string getProgramName( size_t index );
	//-------------------------------------------------------------------------
	/**
	 * Aktiviert Program zu index.
	 * @param index
	 */
	virtual void setProgram( size_t index );
	//-------------------------------------------------------------------------
	/**
	 * @return index des akuell gesetzten Program, falls vorhanden. Andernfalls -1.
	 */
	virtual int getProgram();
	//-------------------------------------------------------------------------
	/**
	 * @return true, if plugin can handle MIDI events
	 */
	virtual bool canHandleMidiEvent() const;
	//-------------------------------------------------------------------------
	virtual void processMidiEvents( sambag::dsp::IMidiEvents * events );
	//-------------------------------------------------------------------------
	virtual size_t getInitialDelay() const;
    //-------------------------------------------------------------------------
    /**
     * @note fills out name, isSynth, uid, vendor, type
     */
    virtual void updatePluginInfo (::processing::PluginInfo &inf) const;
    //-------------------------------------------------------------------------
    virtual void processPlugin( oldPr::Frames::T **,
        oldPr::Frames::T **, size_t numSamples);
    //-------------------------------------------------------------------------
    virtual std::pair<size_t, void*> getStateData() const;
    //-------------------------------------------------------------------------
    virtual void setStateData(size_t size, void* data);
    ///////////////////////////////////////////////////////////////////////////
    // Fields
protected:
    //-------------------------------------------------------------------------
    void parameterChanged(int index);
    //-------------------------------------------------------------------------
    void initParameters();
    //-------------------------------------------------------------------------
    mutable interprocess::PluginSessionClient::Ptr session;
};
}} // namespace(s)

#endif /* SAMBAG_BRIDGEDPLUGIN_H */



