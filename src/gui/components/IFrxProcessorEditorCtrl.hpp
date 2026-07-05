/*
 * IFrxProcessorEditorCtrl.hpp
 *
 *  Created on: Tue Nov  6 10:38:48 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IFRXPROCESSOREDITORCTRL_H
#define SAMBAG_IFRXPROCESSOREDITORCTRL_H

#include <memory>
#include <sambag/disco/components/Forward.hpp>
#include <sambag/com/events/Events.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class IFrxProcessorEditorCtrl.
  */
class IFrxProcessorEditorCtrl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<IFrxProcessorEditorCtrl> Ptr;
protected:
private:
public:
	//-------------------------------------------------------------------------
	/**
	 * called when opening editor window.
	 */
	virtual void open(sdc::WindowPtr win) = 0;
	//-------------------------------------------------------------------------
	/**
	 * called when closing editor window.
	 */
	virtual void close(sdc::WindowPtr win) = 0;
}; // IFrxProcessorEditorCtrl
}}} // namespace(s)

#endif /* SAMBAG_IFRXPROCESSOREDITORCTRL_H */
