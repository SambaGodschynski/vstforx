/*
 * PluginImpl.hpp
 *
 *  Created on: Tue Jan  7 13:30:58 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PLUGINIMPL_H
#define SAMBAG_PLUGINIMPL_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <processing/IHostInfo.h>
#include <processing/IMidiEventProcessor.h>
#include <sambag/dsp/IMidiEvents.hpp>
#include <processing/PlugInfo.h>
#include <processing/Frames.h>
#include <sambag/com/events/Events.hpp>
#include <sambag/com/events/PropertyChanged.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>


namespace processing { namespace parameter {
    class Parameter;
    typedef boost::shared_ptr<Parameter> ParameterPtr;
}}

namespace sambag { namespace disco { namespace components {
    class AWindowImpl;
    typedef boost::shared_ptr<AWindowImpl> AWindowImplPtr;
    typedef boost::weak_ptr<AWindowImpl> AWindowImplWPtr;
    class Window;
    typedef boost::shared_ptr<Window> WindowPtr;
    typedef boost::weak_ptr<Window> WindowWPtr;
}}}

namespace frx { namespace processing {
//=============================================================================
/**
 * @class: PluginArchitectureMissmatch.
 */
//=============================================================================
SAMBAG_DERIVATED_EXCEPTION_CLASS(
    sambag::com::exceptions::IllegalStateException,
    PluginArchitectureMissmatch
);

namespace oldPr = ::processing;
//=============================================================================
/** 
  * @class PluginImpl.
  */
struct APluginImpl :
	public sambag::com::events::EventSender<
        sambag::com::events::PropertyChanged
    >,
    public oldPr::IMidiEventProcessor::EventSender
{
//=============================================================================
    //-------------------------------------------------------------------------
    typedef boost::shared_ptr<APluginImpl> Ptr;
    //-------------------------------------------------------------------------
    typedef boost::weak_ptr<APluginImpl> WPtr;
    //-------------------------------------------------------------------------
    typedef std::pair<int, int> EditorLocation;
    //-------------------------------------------------------------------------
    typedef std::pair<int, int> EditorSize;
    //-------------------------------------------------------------------------
    typedef std::vector<oldPr::parameter::ParameterPtr> Parameters;
    //-------------------------------------------------------------------------
    typedef sambag::disco::components::AWindowImplPtr AWindowImplPtr;
    //-------------------------------------------------------------------------
    typedef sambag::disco::components::AWindowImplWPtr AWindowImplWPtr;
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
	virtual void openEditor(sambag::disco::components::WindowPtr win) = 0;
	//-------------------------------------------------------------------------
	virtual void closeEditor(sambag::disco::components::WindowPtr win) = 0;
	//-------------------------------------------------------------------------
	virtual void onEditorIdle() = 0;
    //-------------------------------------------------------------------------
    virtual bool isAccessable() const = 0;
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
     * @note fills out name, isSynth, uid, vendor, type
     */
    virtual void updatePluginInfo (::processing::PluginInfo &inf) const = 0;
    //-------------------------------------------------------------------------
    virtual void processPlugin( oldPr::Frames::T **,
        oldPr::Frames::T **, size_t numSamples) = 0;
    //-------------------------------------------------------------------------
    virtual ~APluginImpl() {}
    //-------------------------------------------------------------------------
    virtual std::pair<size_t, void*> getStateData() const = 0;
    //-------------------------------------------------------------------------
    virtual void setStateData(size_t size, void* data) = 0;
    //-------------------------------------------------------------------------
    /**
     * @return true if plugin is bridged.
     */
    virtual bool isBridged() const {
        return false;
    }
    //-------------------------------------------------------------------------
    /**
     * @return true if plugin is an intenal impl.
     */
    virtual bool isInternal() const {
        return false;
    }
    //-------------------------------------------------------------------------
    /**
     * @return WindowImpl if the plugin has its own. Can be NULL. 
     * (Bridged plugins have its own impl.)
     */
    virtual AWindowImplPtr getWindowImpl() {
        return AWindowImplPtr();
    }
    //-------------------------------------------------------------------------
    virtual oldPr::IMidiEventProcessor::Connection
    addMidiEventListener(const oldPr::IMidiEventProcessor::EventFunction &f);
    //-------------------------------------------------------------------------
    virtual oldPr::IMidiEventProcessor::Connection
    addTrackedMidiEventListener(const oldPr::IMidiEventProcessor::EventFunction &f,
        oldPr::IMidiEventProcessor::AnyWPtr trackingObject);
    //-------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////
    // Fields
    //-------------------------------------------------------------------------
    IHostInfo::WPtr hostInfo;
    Parameters *parameters;
    std::string location;
    std::string statusMsg;
}; // PluginImpl
}} // namespace(s)

#endif /* SAMBAG_PLUGINIMPL_H */
