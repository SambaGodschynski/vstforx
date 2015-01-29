/*
 * ============================================================================
 * VSTPlugin3x.h
 *      Author: Johannes Unger
 * ============================================================================
 */
#ifndef _VST3XIMPL_H
#define _VST3XIMPL_H

#include "com/one4All.h"
#include "boost/unordered_map.hpp"
#include "OS_Specific/os_processing.h"
#include <string>
#include <sambag/dsp/VstMidiEventAdapter.hpp>
#include <sambag/com/ArithmeticWrapper.hpp>
#include "PluginImpl.hpp"
#include <ivstcomponent.h>
#include <ivsteditcontroller.h>
#include "pluginterfaces/gui/iplugview.h"
#include "base/source/fobject.h"
#include "processing/pluginTypes/VstShellPlugin.hpp"

namespace frx { namespace processing {
namespace oldPr = ::processing;
namespace oldPrPr = ::processing::parameter;
//=============================================================================
/**
 * @class: VST3PluginImpl.
 * Represaentriert ein VST-Plugin.
 */
class VST3PluginImpl: 
	public oldPr::OS_VSTPlugNode3x, // Plattformspezifische impl.
	public APluginImpl,
	public com::Serializable
{
//=============================================================================
public:
	//-------------------------------------------------------------------------
    typedef boost::shared_ptr<VST3PluginImpl> Ptr;
    //-------------------------------------------------------------------------
    typedef boost::weak_ptr<VST3PluginImpl> WPtr;
protected:
	//-------------------------------------------------------------------------
    /**
     * @param 
     * @param the plugin location
     * @param fills container with plugins parameter representations
     */
    VST3PluginImpl(IHostInfo::Ptr hI, const std::string &location,
        Parameters *parameters);
    /**
     * @brief loads a concrete plugin.
     * @param a vst3 specific id
     */
    void createPluginInstance(const std::string &cid);
    /**
     * @brief find out how many plugins are stored in this library.
     * @throws ShellPlugin if more than one found.
     *         It uses the existing VST2X shellplugin handling which ends
     *         with a dialog while catching, where you can select a containing plugin and try
     *         to load again with a specific id: bla.vst3@000AID.
     */
    void determinePluginInstances(oldPr::ShellPluginInfos& _out);
    void unloadPlugin();
    void initController();
    void initParameters();
    void tryCreateEditor();
private:
    Steinberg::Vst::IComponent *plugin;
    Steinberg::Vst::IEditController *controller;
    Steinberg::FObject dummyContext;
    Steinberg::IPlugView *editor;
    std::string cid;
    void valueChanged(void *src, const float &value);
    /**
	 * boolsches Sperren von Parameteraenderungen.
	 * plug => parameter[index] => plug
	 */
	int onPlugChangeParameterIndex;
public:
	//-------------------------------------------------------------------------
	static Ptr create(IHostInfo::Ptr hI, const std::string &location,
        Parameters *parameters);
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
	virtual void openEditor(sambag::disco::components::WindowPtr win);
	//-------------------------------------------------------------------------
	virtual void closeEditor(sambag::disco::components::WindowPtr win);
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
	virtual ~VST3PluginImpl();
    //-------------------------------------------------------------------------
    virtual std::pair<size_t, void*> getStateData() const;
    //-------------------------------------------------------------------------
    virtual void setStateData(size_t size, void* data);
}; // class VST3PluginImpl
}} // namespace processing

#endif


