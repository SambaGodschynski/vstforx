/*
 * FrxParameterLabelUI.hpp
 *
 *  Created on: Thu Nov 15 08:53:50 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPARAMETERLABELUI_H
#define SAMBAG_FRXPARAMETERLABELUI_H

#include <memory>
#include <sambag/disco/components/ui/basic/BasicLabelUI.hpp>
#include <sambag/disco/svg/Style.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxParameterLabelUI.
  */
class FrxParameterLabelUI : public sdcu::basic::BasicLabelUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<FrxParameterLabelUI> Ptr;
	//-------------------------------------------------------------------------
	typedef sdcu::basic::BasicLabelUI Super;
protected:
	//-------------------------------------------------------------------------
	void installDefaults(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	void installListeners(sdc::AComponentPtr c);
private:
	//-------------------------------------------------------------------------
	sds::Style style;
public:
	//-------------------------------------------------------------------------
	virtual void installUI(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new FrxParameterLabelUI());
	}
}; // FrxParameterLabelUI
}}}} // namespace(s)

#endif /* SAMBAG_FRXPARAMETERLABELUI_H */
