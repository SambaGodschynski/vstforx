/*
 * FrxSvgKnobUI.hpp
 *
 *  Created on: Mon Aug 27 10:35:54 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FrxSvgKnobUI_H
#define SAMBAG_FrxSvgKnobUI_H

#include <memory>
#include <gui/components/FrxConcreteIO.hpp>
#include "FrxSvgNodeUI.hpp"
#include <gui/HandyNamespaces.hpp>
#include <sambag/disco/IDiscoFactory.hpp>
#include <sambag/disco/svg/Style.hpp>
#include <gui/components/FrxFontCache.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//=============================================================================
/** 
  * @class FrxSvgKnobUI.
  */
class FrxSvgKnobUI : public FrxSvgNodeUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxSvgNodeUI Super;
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<FrxSvgKnobUI> Ptr;
protected:
	//-------------------------------------------------------------------------
	typedef FrxSvgKnobUI ThisClassType;
	//-------------------------------------------------------------------------
	FrxSvgKnobUI(){}
    //-------------------------------------------------------------------------
    void installKnobListener();
    //-------------------------------------------------------------------------
    sdc::AComponentWPtr main, handle;
    //-------------------------------------------------------------------------
    sdc::SvgComponent::Dummy::WPtr _coronaKnob;
    //-------------------------------------------------------------------------
    virtual void setCoronaAlpha(double alpha);
    //-------------------------------------------------------------------------
    virtual sdc::SvgComponent::Dummy::Ptr getConnectingHandle() const {
        sdc::SvgComponent::Dummy::Ptr res = _coronaKnob.lock();
        SAMBAG_ASSERT(res);
        return res;
    }
public:
	//-------------------------------------------------------------------------
	virtual void createPopupmenuEntries(sdc::PopupMenuPtr menu, 
		FrxCircuidViewPtr view, 
		FrxComponentPtr c)
	{
	}
	//-------------------------------------------------------------------------
	virtual void installUI(sdc::AComponentPtr c);
    //-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res = Ptr(new ThisClassType());
		res->self = res;
		return res;
	}
}; // FrxSvgKnobUI
///////////////////////////////////////////////////////////////////////////////
}}}} // namespace(s)

#endif /* SAMBAG_FrxSvgKnobUI_H */
