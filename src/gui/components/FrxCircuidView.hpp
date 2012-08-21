/*
 * FrxCircuidView.hpp
 *
 *  Created on: Mon Aug 20 12:12:59 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCIRCUIDVIEW_H
#define SAMBAG_FRXCIRCUIDVIEW_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/AContainer.hpp>
#include <sambag/com/ArbitraryType.hpp>

namespace frx { namespace gui { namespace components {
namespace sc = sambag::com;
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxCircuidView.
  */
class FrxCircuidView : public sdc::AContainer {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef sdc::AContainer Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxCircuidView> Ptr;
	//-------------------------------------------------------------------------
	virtual sdcu::AComponentUIPtr getComponentUI(sdcu::ALookAndFeelPtr laf) const;
	//-------------------------------------------------------------------------
	enum ZOrder {
		Z_ProcessorNodes,
		Z_IO,
		Z_Knobs,
		Z_Wires,
		Z_OnTop,
		Z_Default = Z_OnTop
	};
protected:
	//-------------------------------------------------------------------------
	FrxCircuidView();
	//-------------------------------------------------------------------------
	virtual void constructorAlt();
private:
public:
	//-------------------------------------------------------------------------
	virtual void add(sdc::AComponentPtr comp, ZOrder zord = Z_Default);
	//-------------------------------------------------------------------------
	virtual void remove(sdc::AComponentPtr comp, ZOrder zord);
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(FrxCircuidView)
}; // FrxCircuidView
}}} // namespace(s)

#endif /* SAMBAG_FRXCIRCUIDVIEW_H */
