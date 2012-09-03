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
namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
//namespace sdcub = sdcu::basic;
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
	void installComponents();
	//-------------------------------------------------------------------------
	void installDefaults();
private:
public:
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new FrxLookAndFeel());
	}
}; // FrxLookAndFeel
}}}} // namespace(s)

#endif /* SAMBAG_FRXLOOKANDFEEL_H */
