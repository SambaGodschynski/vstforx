/*
 * SetupCtrl.hpp
 *
 *  Created on: Thu Oct 25 13:56:01 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_SETUPCTRL_H
#define SAMBAG_SETUPCTRL_H

#include <boost/shared_ptr.hpp>
#include <string>
namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class SetupCtrl.
  */
class SetupCtrl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<SetupCtrl> Ptr;
protected:
	//-------------------------------------------------------------------------
	SetupCtrl(){}
private:
public:
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new SetupCtrl());
	}
	//-------------------------------------------------------------------------
	std::string selectDirectory(const std::string &startDir="") const;
}; // SetupCtrl
}}} // namespace(s)

#endif /* SAMBAG_SETUPCTRL_H */
