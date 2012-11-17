/*
 * FrxParameterLabelUI.hpp
 *
 *  Created on: Thu Nov 15 08:53:50 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPARAMETERLABELUI_H
#define SAMBAG_FRXPARAMETERLABELUI_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/ui/basic/BasicLabelUI.hpp>
namespace frx { namespace gui {
namespace components { namespace ui { 
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxParameterLabelUI.
  */
class FrxParameterLabelUI : public sdcu::basic::BasicLabelUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxParameterLabelUI> Ptr;
	//-------------------------------------------------------------------------
	typedef sdcu::basic::BasicLabelUI Super;
protected:
private:
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
