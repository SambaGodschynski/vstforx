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
#include <string>

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
	typedef float ZOrder;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxCircuidView> Ptr;
	//-------------------------------------------------------------------------
	virtual sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const;
	//-------------------------------------------------------------------------
	static const std::string PROPERTY_ZORDER;
	//-------------------------------------------------------------------------
	static const float Z_ProcessorNodes;
	//-------------------------------------------------------------------------
	static const float Z_IO;
	//-------------------------------------------------------------------------
	static const float Z_Knobs;
	//-------------------------------------------------------------------------
	static const float Z_Wires;
	//-------------------------------------------------------------------------
	static const float Z_OnTop;
	//-------------------------------------------------------------------------
	static const float Z_Default;
protected:
	//-------------------------------------------------------------------------
	FrxCircuidView();
	//-------------------------------------------------------------------------
	virtual void constructorAlt();
private:
public:
	//-------------------------------------------------------------------------
	/**
	 * @note: (Z)Orders is done during insert. So avoid frequently add/remove.
	 */
	virtual void add(sdc::AComponentPtr comp, ZOrder zord = Z_Default);
	//-------------------------------------------------------------------------
	virtual void remove(sdc::AComponentPtr comp);
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(FrxCircuidView)
}; // FrxCircuidView
}}} // namespace(s)

#endif /* SAMBAG_FRXCIRCUIDVIEW_H */
