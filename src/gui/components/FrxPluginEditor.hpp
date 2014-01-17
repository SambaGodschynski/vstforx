/*
 * FrxPluginEditor.hpp
 *
 *  Created on: Tue Nov  6 10:27:25 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPLUGINEDITOR_H
#define SAMBAG_FRXPLUGINEDITOR_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
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
	typedef boost::shared_ptr<FrxPluginEditor> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxPluginEditor> WPtr;
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
public:
    //-------------------------------------------------------------------------
	SAMBAG_STD_WINDOW_CREATOR(FrxPluginEditor)
    //-------------------------------------------------------------------------
	static Ptr create(sdc::AWindowImpl::Ptr winImpl,
        sdc::Window::Ptr parentWindow=sdc::Window::Ptr());
    //-------------------------------------------------------------------------
    ~FrxPluginEditor();
}; // FrxPluginEditor
}}} // namespace(s)

#endif /* SAMBAG_FRXPLUGINEDITOR_H */
