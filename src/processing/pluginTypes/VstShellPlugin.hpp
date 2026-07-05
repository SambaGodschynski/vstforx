/*
 * VstShellPlugin.hpp
 *
 *  Created on: Wed Dec  5 15:24:48 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_VSTSHELLPLUGIN_H
#define SAMBAG_VSTSHELLPLUGIN_H

#include <memory>
#include <string>
#include <list>
#include <boost/lexical_cast.hpp>

namespace processing {
//============================================================================================================
// Klasse: ShellPluginException.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
struct ShellPluginInfo {
    std::string id;
    std::string name;
	ShellPluginInfo(const std::string &name="", int id=0) :
        name(name),
        id(boost::lexical_cast<std::string>(id))
    {
    }
    ShellPluginInfo(const std::string &name, const std::string &id) :
        name(name),
        id(id)
    {
    }
};
//------------------------------------------------------------------------------------------------------------
typedef std::list<ShellPluginInfo> ShellPluginInfos;
//------------------------------------------------------------------------------------------------------------
/**
 * @class ShellPluginException
 * Plugin is a Shellplugin.
 */
struct ShellPluginException {
	std::string plugname;
	ShellPluginInfos content;
	ShellPluginException( const std::string &plugname,
		const ShellPluginInfos &content) : plugname(plugname), content(content) 
	{
	}
};

} // namespace(s)

#endif /* SAMBAG_VSTSHELLPLUGIN_H */
