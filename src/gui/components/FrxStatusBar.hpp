/*
 * FrxStatusBar.hpp
 *
 *  Created on: Thu May 30 10:54:18 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXSTATUSBAR_H
#define SAMBAG_FRXSTATUSBAR_H

#include <memory>
#include <string>
#include <sambag/disco/components/Label.hpp>
#include <sambag/disco/components/Panel.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxStatusBar.
  */
class FrxStatusBar : public sdc::Panel {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<FrxStatusBar> Ptr;
protected:
	//-------------------------------------------------------------------------
	virtual void initStatusBar();
	//-------------------------------------------------------------------------
	virtual void postConstructor();
private:
	//-------------------------------------------------------------------------
	sdc::LabelPtr statusMessage;
public:
	//-------------------------------------------------------------------------
	void setStatusMessage(const std::string &txt, const std::string &iconname);
	//-------------------------------------------------------------------------
	sdc::Label::Ptr getStatusLabel() const;
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(FrxStatusBar)
}; // FrxStatusBar
}}} // namespace(s)

#endif /* SAMBAG_FRXSTATUSBAR_H */
