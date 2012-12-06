/*
 * VstShellPlugin.hpp
 *
 *  Created on: Wed Dec  5 15:24:48 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_VSTSHELLPLUGIN_H
#define SAMBAG_VSTSHELLPLUGIN_H

#include <boost/shared_ptr.hpp>
#include <string>
#include <list>

namespace processing {
//============================================================================================================
// Klasse: ShellPluginException.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
struct ShellPluginInfo {
	std::string name;
	int id;
	ShellPluginInfo(const std::string &name="", int id=0) : name(name), id(id) {}
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
