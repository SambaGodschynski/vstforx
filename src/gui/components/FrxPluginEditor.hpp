/*
 * FrxPluginEditor.hpp
 *
 *  Created on: Tue Nov  6 10:27:25 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPLUGINEDITOR_H
#define SAMBAG_FRXPLUGINEDITOR_H

#include <boost/shared_ptr.hpp>
#include "FrxProcessorEditor.hpp"

namespace frx { namespace gui { namespace components {
namespace sce = sambag::com::events;
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
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
protected:
	//-------------------------------------------------------------------------
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	FrxPluginEditor(sdc::Window::Ptr parent = sdc::Window::Ptr()) :
		 Super(parent)
	{
	}
private:
public:
	SAMBAG_STD_WINDOW_CREATOR(FrxPluginEditor)
}; // FrxPluginEditor
}}} // namespace(s)

#endif /* SAMBAG_FRXPLUGINEDITOR_H */
