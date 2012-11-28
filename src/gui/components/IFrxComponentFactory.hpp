/*
 * IFrxComponentFactory.hpp
 *
 *  Created on: Thu Nov  1 10:17:09 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IFRXCOMPONENTFACTORY_H
#define SAMBAG_IFRXCOMPONENTFACTORY_H

#include "Forward.hpp"
#include <boost/function.hpp>
#include <string>
#include <list>
#include <processing/PlugInfo.h>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class IFrxComponentFactory <Singleton>.
  * Serves a list of component names and its creator functions which
  * creates a specific model object and the related view object.
  */
class IFrxComponentFactory {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::function<FrxProcessorNodePtr(FrxCircuidViewPtr)> 
	ProcessorCreator;
	//-------------------------------------------------------------------------
	typedef boost::function<
		FrxProcessorNodePtr(FrxCircuidViewPtr, ::processing::PluginInfo)
	> PluginCreator;
	//-------------------------------------------------------------------------
	typedef boost::function<
		FrxParameterPtr(FrxCircuidViewPtr)
	> FreeParameterCreator;
	//-------------------------------------------------------------------------
	typedef boost::function<
		FrxParameterPtr(FrxCircuidViewPtr, int)
	> HostParameterCreator;
	//-------------------------------------------------------------------------
	virtual PluginCreator getPluginCreator() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return a creator for the processors name.
	 * @see FrxComponent::getName()
	 */
	virtual ProcessorCreator 
	getProcessorCreator(const std::string &name) const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return number of processor creators.
	 */
	virtual void getProcessorNames(std::list<std::string> &out) const = 0;
	//-------------------------------------------------------------------------
	virtual FreeParameterCreator getFreeParameterCreator() const = 0;
	//-------------------------------------------------------------------------
	virtual HostParameterCreator getHostParameterCreator() const = 0;

}; // IFrxComponentFactory
///////////////////////////////////////////////////////////////////////////////
extern IFrxComponentFactory & getComponentFactory(FrxCircuidViewPtr view);
}}} // namespace(s)

#endif /* SAMBAG_IFRXCOMPONENTFACTORY_H */
