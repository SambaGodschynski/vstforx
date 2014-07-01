/*
 * FrxSvgIOUI.hpp
 *
 *  Created on: Mon Aug 27 10:35:54 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FrxSvgIOUI_H
#define SAMBAG_FrxSvgIOUI_H

#include <boost/shared_ptr.hpp>
#include <gui/components/FrxConcreteIO.hpp>
#include "FrxSvgNodeUI.hpp"
#include <gui/HandyNamespaces.hpp>
#include <sambag/disco/IDiscoFactory.hpp>
#include <sambag/disco/svg/graphicElements/Style.hpp>
#include <gui/components/FrxFontCache.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//=============================================================================
/** 
  * @class FrxSvgIOUI.
  */
class FrxSvgIOUI : public FrxSvgNodeUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxSvgNodeUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxSvgIOUI> Ptr;
protected:
	//-------------------------------------------------------------------------
	typedef FrxSvgIOUI ThisClassType;
	//-------------------------------------------------------------------------
	FrxSvgIOUI(){}
public:
	//-------------------------------------------------------------------------
	virtual void createPopupmenuEntries(sdc::PopupMenuPtr menu, 
		FrxCircuidViewPtr view, 
		FrxComponentPtr c)
	{
	}
	//-------------------------------------------------------------------------
	virtual void installUI(sdc::AComponentPtr c) {
        Super::installUI(c);
    }
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res = Ptr(new ThisClassType());
		res->self = res;
		return res;
	}
	//-------------------------------------------------------------------------
	virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {}
}; // FrxSvgIOUI
///////////////////////////////////////////////////////////////////////////////
}}}} // namespace(s)

#endif /* SAMBAG_FrxSvgIOUI_H */
