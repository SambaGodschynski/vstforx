/*
 * FrxTooltipManager.hpp
 *
 *  Created on: Sun Nov 25 15:29:13 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXTOOLTIPMANAGER_H
#define SAMBAG_FRXTOOLTIPMANAGER_H

#include <loki/Singleton.h>
#include <sambag/disco/components/DefaultTooltipManager.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxTooltipManager.
  */
class FrxTooltipManager : 
	public sambag::disco::components::DefaultTooltipManager 
{
//=============================================================================
friend struct Loki::CreateUsingNew<FrxTooltipManager>;
public:
	//-------------------------------------------------------------------------
	typedef sambag::disco::components::DefaultTooltipManager Super;
protected:
	//-------------------------------------------------------------------------
	FrxTooltipManager(){}
	//-------------------------------------------------------------------------
	virtual void showTooltip(sambag::disco::components::AComponentPtr c);
private:
public:
	//-------------------------------------------------------------------------
	virtual void 
	registerComponent(sambag::disco::components::AComponentPtr component);
	//-------------------------------------------------------------------------
	~FrxTooltipManager(){}
	//-------------------------------------------------------------------------
	static FrxTooltipManager & instance();
	//-------------------------------------------------------------------------
	virtual void 
		mouseEntered(const sambag::disco::components::events::MouseEvent &ev);
	//-------------------------------------------------------------------------
	virtual void 
		mouseExited(const sambag::disco::components::events::MouseEvent &ev);
}; // FrxTooltipManager
}}} // namespace(s)

#endif /* SAMBAG_FRXTOOLTIPMANAGER_H */
