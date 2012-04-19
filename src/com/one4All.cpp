/*
 * ===========================================================================================================
 * one4All.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "one4All.h"
#include "OS_Specific/OS_com.h"
#include <boost/xpressive/xpressive.hpp>
#include <sstream>


namespace com {
//============================================================================================================
bool isSubDirectory ( const sambag::com::Location &parent,  const sambag::com::Location &sub ) {
	string p = parent.string();
	string s = sub.string();
	if ( p==s ) return false;
	return s.find (p) != string::npos;
}
//------------------------------------------------------------------------------------------------------------
std::pair<std::string, int> extractVSTPluginFilename(const std::string &filename) {
	using namespace boost::xpressive;
	mark_tag tFilename(1), tShellid(2);
	// (.+?)@{0,1}\d+$
	cregex pat = (tFilename= -+_) >> "@" >> (tShellid= +_d) >> eol;
	cmatch what;
	if(regex_search(filename.c_str(), what, pat)) { // matches filename@number
		std::stringstream ss;
		int shellId;
		ss << what[tShellid];
		ss >> shellId;
		return std::make_pair(
			what[tFilename],
			shellId
		);
	}
	// no match: return whole string
	return std::make_pair(filename, 0);
}
//------------------------------------------------------------------------------------------------------------
std::string createVSTPluginFilename(const std::string &filename, int shellId) {
	if (shellId == 0)
		return filename;
	std::stringstream ss;
	ss<<filename<<"@"<<shellId;
	return ss.str();
}
} // namespace com