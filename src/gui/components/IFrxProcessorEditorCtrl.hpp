/*
 * IFrxProcessorEditorCtrl.hpp
 *
 *  Created on: Tue Nov  6 10:38:48 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IFRXPROCESSOREDITORCTRL_H
#define SAMBAG_IFRXPROCESSOREDITORCTRL_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/Forward.hpp>
#include <sambag/com/events/Events.hpp>
namespace frx { namespace gui { namespace components {
namespace sce = sambag::com::events;
namespace sd = sambag::disco;
namespace sdc = sd::components;
//=============================================================================
/** 
  * @class IFrxProcessorEditorCtrl.
  */
class IFrxProcessorEditorCtrl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IFrxProcessorEditorCtrl> Ptr;
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
