/*
 * PluginImpl.hpp
 *
 *  Created on: Tue Jan  7 13:30:58 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PLUGINIMPL_H
#define SAMBAG_PLUGINIMPL_H

#include <boost/shared_ptr.hpp>
#include <processing/IHostInfo.h>
#include <sambag/dsp/IMidiEvents.hpp>
#include <processing/PlugInfo.h>
#include <processing/Frames.h>

namespace processing {
    namespace parameter {
        class Parameter;
        typedef boost::shared_ptr<Parameter> ParameterPtr;
    }
}

namespace frx { namespace processing {
namespace oldPr = ::processing;
//=============================================================================
/** 
  * @class PluginImpl.
  */
struct APluginImpl {
//=============================================================================
    //-------------------------------------------------------------------------
    typedef std::vector<oldPr::parameter::ParameterPtr> Parameters;
    //-------------------------------------------------------------------------
    /**
     * @param 
     * @param the plugin location
     * @param fills container with plugins parameter representations
     */
    APluginImpl(IHostInfo::Ptr hI,
        const std::string &location,
        Parameters *parameters
    );
    //-------------------------------------------------------------------------
    virtual void baseConfigChanged() = 0;
    //-------------------------------------------------------------------------
    virtual void turnOff() = 0;
    //-------------------------------------------------------------------------
    virtual void turnOn() = 0;
    //-------------------------------------------------------------------------
    virtual void openPlugin() = 0;
    //-------------------------------------------------------------------------
    virtual void closePlugin() = 0;
    //-------------------------------------------------------------------------
    virtual size_t getNumInputChannels() const = 0;
    //-------------------------------------------------------------------------
    virtual size_t getNumOutputChannels() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return true, wenn Plugin ueber Editor verfuegt.
	 */
	virtual bool hasEditor() const = 0;
	//-------------------------------------------------------------------------
	virtual void openEditor(void *window) const = 0;
	//-------------------------------------------------------------------------
	virtual void closeEditor(void *window) const = 0;
	//-------------------------------------------------------------------------
	virtual void onEditorIdle() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return Anzahl aller Plugin-Programme (aka. Presets)
	 */
	virtual size_t getNumPrograms() = 0;
	//-------------------------------------------------------------------------
	/**
	 * @param index
	 * @return Program-Name zu index.
	 */
	virtual std::string getProgramName( size_t index ) = 0;
	//-------------------------------------------------------------------------
	/**
	 * Aktiviert Program zu index.
	 * @param index
	 */
	virtual void setProgram( size_t index ) = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return index des akuell gesetzten Program, falls vorhanden. Andernfalls -1.
	 */
	virtual int getProgram() = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return true, if plugin can handle MIDI events
	 */
	virtual bool canHandleMidiEvent() const = 0;
	//-------------------------------------------------------------------------
	virtual void processMidiEvents( sambag::dsp::IMidiEvents * events ) = 0;
	//-------------------------------------------------------------------------
	virtual size_t getInitialDelay() const = 0;
    //-------------------------------------------------------------------------
    /**
     * @note does not affect fills out name, isSynth, uid, vendor
     */
    void getPluginInfo (::processing::PluginInfo &inf) const;
    //-------------------------------------------------------------------------
    virtual void processPlugin( oldPr::Frames::T **, oldPr::Frames::T **) = 0;
    ///////////////////////////////////////////////////////////////////////////
    // Fields
    //-------------------------------------------------------------------------
    IHostInfo::Ptr hI;
    Parameters *parameters;
}; // PluginImpl
}} // namespace(s)

#endif /* SAMBAG_PLUGINIMPL_H */
