/*
 * FrxKnobUI.hpp
 *
 *  Created on: Mon Feb 18 15:05:35 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXKNOBUI_H
#define SAMBAG_FRXKNOBUI_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/ui/basic/BasicKnobUI.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxKnobUI.
  */
template <class ComponentModel>
class FrxKnobUI : public sdcu::basic::BasicKnobUI<ComponentModel> {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxKnobUI> Ptr;
	//-------------------------------------------------------------------------
	typedef FrxKnobUI<ComponentModel> Class;
	//-------------------------------------------------------------------------
	typedef sdcu::basic::BasicKnobUI<ComponentModel> Super;
protected:
private:
public:
	//-------------------------------------------------------------------------
	virtual void installUI(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	/**
	 * @returns true if p hits knob concerning c's parent componentui.
	 */
	virtual bool contains(sdc::AComponentPtr c, const sd::Point2D &p);
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res(new Class());
		res->self = res;
		return res;
	}
}; // FrxKnobUI
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class C>
void FrxKnobUI<C>::installUI(sdc::AComponentPtr c) {
    Super::installUI(c);
}
//-----------------------------------------------------------------------------
template <class C>
bool FrxKnobUI<C>::contains(sdc::AComponentPtr c, const sd::Point2D &p) {
	return Super::contains(c, p);
}
}}}} // namespace(s)

#endif /* SAMBAG_FRXKNOBUI_H */
