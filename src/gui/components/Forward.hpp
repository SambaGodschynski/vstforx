/*
 * Forward.hpp
 *
 *  Created on: Thu Sep 27 14:49:27 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXFORWARD_H
#define SAMBAG_FRXFORWARD_H


#include <boost/shared_ptr.hpp>

namespace frx { namespace gui { namespace components {
	class FrxCircuidView;
	typedef boost::shared_ptr<FrxCircuidView> FrxCircuidViewPtr; 
	typedef boost::weak_ptr<FrxCircuidView> FrxCircuidViewWPtr; 
	class FrxSelection;
	typedef boost::shared_ptr<FrxSelection> FrxSelectionPtr; 
	typedef boost::weak_ptr<FrxSelection> FrxSelectionWPtr; 
	class FrxNode;
	typedef boost::shared_ptr<FrxNode> FrxNodePtr; 
	typedef boost::weak_ptr<FrxNode> FrxNodeWPtr; 
}}}

#endif