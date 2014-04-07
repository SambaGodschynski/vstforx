/*
 * FrxMenuLabel.hpp
 *
 *  Created on: Thu Apr  3 11:37:15 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXMENULABEL_H
#define SAMBAG_FRXMENULABEL_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/disco/components/Label.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxMenuLabel.
  * Some tweaks to basic label to fit in context menu
  */
class FrxMenuLabel : public sdc::Label {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef sdc::Label Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxMenuLabel> Ptr;
	typedef boost::weak_ptr<FrxMenuLabel> WPtr;
protected:
	//-------------------------------------------------------------------------
	FrxMenuLabel();
    //-------------------------------------------------------------------------
    virtual void postConstructor();
private:
public:
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(FrxMenuLabel)
    //-------------------------------------------------------------------------
    virtual sd::Dimension getPreferredSize();
}; // FrxMenuLabel
}}} // namespace(s)

#endif /* SAMBAG_FRXMENULABEL_H */
