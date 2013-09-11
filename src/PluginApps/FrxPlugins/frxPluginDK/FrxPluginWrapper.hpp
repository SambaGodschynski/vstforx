/*
 * FrxPluginWrapper.hpp
 *
 *  Created on: Tue Sep 10 15:32:06 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPLUGINWRAPPER_H
#define SAMBAG_FRXPLUGINWRAPPER_H

#include "FrxPlugInterface.h"
#include <string>
#include <sstream>
#include <sambag/dsp/DspPlugin.hpp>
#include <sambag/com/Exception.hpp>
#include <algorithm>


namespace sambag { namespace dsp { namespace frx {
//=============================================================================
/** 
  * @class FrxPluginWrapper
  * TODO: impl. dispatcher, processing
  */
template <
	class _PluginProcessor,
	class _PluginTraits = StdPluginTraits<2,2,false>,
	class _CreateEditorPolicy = CreateNoEditor
> 
class FrxPluginWrapper :
    public _PluginProcessor,
	public _PluginTraits,
	public _CreateEditorPolicy,
	public IHost
{
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef _PluginProcessor PluginProcessor;
    //-------------------------------------------------------------------------
    typedef _PluginTraits PluginTraits;
    //-------------------------------------------------------------------------
    typedef _CreateEditorPolicy CreateEditorPolicy;
    //-------------------------------------------------------------------------
    typedef FrxPluginWrapper<PluginProcessor, PluginTraits, CreateEditorPolicy>
        ThisClass;
protected:
    //-------------------------------------------------------------------------
    RawFrxPlugin rawPlugin;
private:
public:
    //-------------------------------------------------------------------------
    FrxPluginWrapper() {
        // TODO: setup rawPlugin
    }
    //-------------------------------------------------------------------------
    const RawFrxPlugin * getRawFrxPlugin() const {
        return &rawPlugin;
    }
    //-------------------------------------------------------------------------
    RawFrxPlugin * getRawFrxPlugin() {
        return &rawPlugin;
    }
    ///////////////////////////////////////////////////////////////////////////
    // PluginProcessor to host;
    //-------------------------------------------------------------------------
	virtual void delayChanged(int delaySamples) {
    }
    //-------------------------------------------------------------------------
	virtual void ioConfigurationChanged(int numInputs, int numOutputs) {
    }
    //-------------------------------------------------------------------------
	virtual void parameterChanged(int index) {
    }
    //-------------------------------------------------------------------------
	virtual HostTimeInfo * getHostTimeInfo (int filter) {
        return NULL;
    }
    //-------------------------------------------------------------------------
	/**
	 * @return true if succeed.
	 */
	virtual bool requestEditorResize(int witdh, int height) {
        return false;
    }
    //-------------------------------------------------------------------------
	/**
	 * @return editor. Is NULL if no editor which extends IEditor is used.
	 */
	virtual IEditor * getEditor() const {
        return NULL;
    }
    ///////////////////////////////////////////////////////////////////////////
    FrxInteger dispatch( RawFrxPlugin *effect, FrxInteger opcode,
        FrxInteger index, FrxInteger value, void *ptr, float opt);
    ///////////////////////////////////////////////////////////////////////////
    static FrxInteger dispatcher( RawFrxPlugin *effect, FrxInteger opcode,
        FrxInteger index, FrxInteger value, void *ptr, float opt)
    {
        if (!effect) {
            return -1;
        }
        SAMBAG_ASSERT(effect->object);
        
        
        ThisClass *plug = static_cast<ThisClass*>( effect->object );
        
        // TODO: if opcode == FrxEffectDestroy
        // plug->destroy()
        // delete plug
        
        return plug->dispatch(effect, opcode, index, value, ptr, opt);
    }
}; // FrxPlugin
}}} // namespace(s)

#endif /* SAMBAG_FRXPLUGINWRAPPER_H */
