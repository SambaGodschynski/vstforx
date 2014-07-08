/*
 * FrxSvgPluginUI.hpp
 *
 *  Created on: Mon Aug 27 10:35:54 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FrxSvgPluginUI_H
#define SAMBAG_FrxSvgPluginUI_H

#include <boost/shared_ptr.hpp>
#include <gui/components/FrxConcreteIO.hpp>
#include "FrxSvgProcessorUI.hpp"
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
  * @class FrxSvgPluginUI.
  */
class FrxSvgPluginUI : public FrxSvgProcessorUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxSvgProcessorUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxSvgPluginUI> Ptr;
protected:
	//-------------------------------------------------------------------------
	typedef FrxSvgPluginUI ThisClassType;
	//-------------------------------------------------------------------------
	FrxSvgPluginUI(){}
public:
	//-------------------------------------------------------------------------
	virtual void installListeners(sdc::AComponent::Ptr c);
	//-------------------------------------------------------------------------
	virtual void createPopupmenuEntries(sdc::PopupMenuPtr menu, 
		FrxCircuidViewPtr view, 
		FrxComponentPtr c)
	{
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

}; // FrxSvgPluginUI
///////////////////////////////////////////////////////////////////////////////
}}}} // namespace(s)

#endif /* SAMBAG_FrxSvgPluginUI_H */
