/*
 * FrxPluginEditor.hpp
 *
 *  Created on: Tue Nov  6 10:27:25 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPLUGINEDITOR_H
#define SAMBAG_FRXPLUGINEDITOR_H

#include <memory>
#include "FrxProcessorEditor.hpp"
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxPluginEditor.
  */
class FrxPluginEditor : public FrxProcessorEditor {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxProcessorEditor Super; 
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<FrxPluginEditor> Ptr;
	//-------------------------------------------------------------------------
	typedef std::weak_ptr<FrxPluginEditor> WPtr;
protected:
	//-------------------------------------------------------------------------
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	FrxPluginEditor(sdc::Window::Ptr parent = sdc::Window::Ptr()) :
		 Super(parent)
	{
	}
	//-------------------------------------------------------------------------
	FrxPluginEditor(sdc::AWindowImpl::Ptr impl, sdc::Window::Ptr parent = sdc::Window::Ptr()) :
		 Super(impl, parent)
	{
	}

private:
    //-------------------------------------------------------------------------
    bool isRaw;
public:
    //-------------------------------------------------------------------------
	static Ptr create(
        sdc::Window::Ptr parentWindow=sdc::Window::Ptr(),
        bool isRaw = true
    );
    //-------------------------------------------------------------------------
	static Ptr create(sdc::AWindowImpl::Ptr winImpl,
        sdc::Window::Ptr parentWindow=sdc::Window::Ptr());
    //-------------------------------------------------------------------------
    ~FrxPluginEditor();
}; // FrxPluginEditor
}}} // namespace(s)

#endif /* SAMBAG_FRXPLUGINEDITOR_H */
