/*
 * FrxSvgProcessorUI.hpp
 *
 *  Created on: Mon Aug 27 10:35:54 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FrxSvgProcessorUI_H
#define SAMBAG_FrxSvgProcessorUI_H

#include <boost/shared_ptr.hpp>
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
  * @class FrxSvgProcessorUI.
  */
class FrxSvgProcessorUI : public FrxSvgNodeUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxSvgNodeUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxSvgProcessorUI> Ptr;
protected:
	//-------------------------------------------------------------------------
	typedef FrxSvgProcessorUI ThisClassType;
	//-------------------------------------------------------------------------
	FrxSvgProcessorUI(){}
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
	//-------------------------------------------------------------------------
	virtual void beginConnecting(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	virtual void connecting(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	virtual void endConnecting(const sdc::events::MouseEvent &ev) {}
	//-------------------------------------------------------------------------
	virtual void use(const sdc::events::MouseEvent &ev) {}

}; // FrxSvgProcessorUI
///////////////////////////////////////////////////////////////////////////////
}}}} // namespace(s)

#endif /* SAMBAG_FrxSvgProcessorUI_H */
