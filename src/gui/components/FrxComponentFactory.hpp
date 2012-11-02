/*
 * FrxComponentFactory.hpp
 *
 *  Created on: Thu Nov  1 10:17:13 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCOMPONENTFACTORY_H
#define SAMBAG_FRXCOMPONENTFACTORY_H

#include <boost/shared_ptr.hpp>
#include "IFrxComponentFactory.hpp"
#include <loki/Singleton.h>
#include <map>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxComponentFactory.
  */
class FrxComponentFactory : public IFrxComponentFactory {
//=============================================================================
friend struct Loki::CreateUsingNew<FrxComponentFactory>;
public:
protected:
	//-------------------------------------------------------------------------
	FrxComponentFactory();
	//-------------------------------------------------------------------------
	virtual void initMap();
private:
	//-------------------------------------------------------------------------
	typedef std::map<std::string, ProcessorCreator> ProcessorMap;
	//-------------------------------------------------------------------------
	ProcessorMap processorMap;
public:
	//-------------------------------------------------------------------------
	virtual PluginCreator getPluginCreator() const;
	//-------------------------------------------------------------------------
	/**
	 * @return a creator for the processors name.
	 * @see FrxComponent::getName()
	 */
	virtual ProcessorCreator 
	getProcessorCreator(const std::string &name) const;
	//-------------------------------------------------------------------------
	/**
	 * @return number of processor creators.
	 */
	virtual void getProcessorNames(std::list<std::string> &out) const;
	//-------------------------------------------------------------------------
	virtual FreeParameterCreator getFreeParameterCreator() const;
	//-------------------------------------------------------------------------
	virtual HostParameterCreator getHostParameterCreator() const;
}; // FrxComponentFactory
}}} // namespace(s)

#endif /* SAMBAG_FRXCOMPONENTFACTORY_H */
