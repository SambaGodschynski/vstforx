/*
 * FrxColumnBrowserUI.hpp
 *
 *  Created on: Mon Oct 22 11:50:10 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPROCESSORBROWSERUI_H
#define SAMBAG_FRXPROCESSORBROWSERUI_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/ui/AComponentUI.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxColumnBrowserUI.
  */
class FrxColumnBrowserUI : public sdc::ui::AComponentUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxColumnBrowserUI> Ptr;
protected:
	//-------------------------------------------------------------------------
	virtual void installListeners(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void installDefaults(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void installUI(sdc::AComponentPtr c);
public:
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res(new FrxColumnBrowserUI());
		return res;
	}
	//-------------------------------------------------------------------------
	virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);
private:
public:
}; // FrxColumnBrowserUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSORBROWSERUI_H */
