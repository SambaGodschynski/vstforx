/*
 * FrxLookAndFeel
 *
 *  Created on: Tue Aug 17 17:33:20 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXLOOKANDFEEL_H
#define SAMBAG_FRXLOOKANDFEEL_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/ui/basic/BasicLookAndFeel.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class FrxLookAndFeel.
  */
class FrxLookAndFeel : public sdcu::basic::BasicLookAndFeel {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef sdcu::basic::BasicLookAndFeel Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxLookAndFeel> Ptr;
protected:
	//-------------------------------------------------------------------------
	FrxLookAndFeel();
	//-------------------------------------------------------------------------
	virtual void installComponents();
	//-------------------------------------------------------------------------
	virtual void installDefaults();
	//-------------------------------------------------------------------------
	void installTooltipManager();
private:
public:
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res(new FrxLookAndFeel());
		res->installLookAndFeel();
		return res;
	}
}; // FrxLookAndFeel
}}}} // namespace(s)

#endif /* SAMBAG_FRXLOOKANDFEEL_H */
